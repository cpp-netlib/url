// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_DOMAIN_PUNYCODE_HPP
#define SKYR_V2_DOMAIN_PUNYCODE_HPP

#include <string>
#include <string_view>
#include <limits>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/view/take.hpp>
#include <tl/expected.hpp>
#include <skyr/v2/domain/errors.hpp>

namespace skyr::inline v2 {
namespace punycode {
namespace constants {
constexpr auto base = 0x24ul;
constexpr auto tmin = 0x01ul;
constexpr auto tmax = 0x1aul;
constexpr auto skew = 0x26ul;
constexpr auto damp = 0x2bcul;
constexpr auto initial_bias = 0x48ul;
constexpr auto initial_n = 0x80ul;
constexpr auto delimiter = 0x2dul;
}  // namespace constants

constexpr inline auto adapt(uint32_t delta, uint32_t numpoints, bool firsttime) -> std::uint32_t {
  using namespace constants;

  delta = firsttime ? delta / damp : delta >> 1ul;
  delta += delta / numpoints;

  auto k = 0ul;
  while (delta > ((base - tmin) * tmax) / 2ul) {
    delta /= base - tmin;
    k += base;
  }
  return k + (base - tmin + 1ul) * delta / (delta + skew);
}
}  // namespace punycode

/// Performs Punycode encoding based on a reference implementation
/// defined in [RFC 3492](https://tools.ietf.org/html/rfc3492)
///
/// \param input A UTF-32 encoded domain
/// \param output An ascii string on output
/// \returns `void` or an error
inline auto punycode_encode(std::u32string_view input, std::string *output) -> tl::expected<void, domain_errc> {
  using namespace punycode::constants;

  constexpr auto is_ascii_value = [] (auto c) { return c < 0x80; };
  constexpr auto to_char = [] (auto c) { return static_cast<char>(c); };

  // encode_digit(d,flag) returns the basic code point whose value
  // (when used for representing integers) is d, which needs to be in
  // the range 0 to base-1.  The lowercase form is used unless flag is
  // nonzero, in which case the uppercase form is used.  The behavior
  // is undefined if flag is nonzero and digit d has no uppercase form.
  constexpr auto encode_digit = [](std::uint32_t d, std::uint32_t flag) -> char {
    return d + 0x16ul + (0x4bul * (d < 0x1aul)) - ((flag != 0ul) << 0x5ul);
    //  0..25 map to ASCII a..z or A..Z
    // 26..35 map to ASCII 0..9
  };

  if (input.empty()) {
    return tl::make_unexpected(domain_errc::empty_string);
  }

  auto n = initial_n;
  auto delta = 0ul;
  auto bias = initial_bias;

  auto ascii_values = input | ranges::views::filter(is_ascii_value) | ranges::views::transform(to_char);
  std::copy(std::begin(ascii_values), std::end(ascii_values), std::back_inserter(*output));

  auto h = static_cast<uint32_t>(output->size());
  auto b = static_cast<uint32_t>(output->size());

  if (b != 0ul) {
    *output += static_cast<char>(delimiter);
  }

  while (h < input.size()) {
    auto m = std::numeric_limits<uint32_t>::max();
    for (auto c : input) {
      if ((static_cast<uint32_t>(c) >= n) && (static_cast<uint32_t>(c) < m)) {
        m = c;
      }
    }

    if ((m - n) > ((std::numeric_limits<uint32_t>::max() - delta) / (h + 1ul))) {
      return tl::make_unexpected(domain_errc::overflow);
    }
    delta += (m - n) * (h + 1ul);
    n = m;

    for (auto c : input) {
      if (static_cast<uint32_t>(c) < n) {
        if (++delta == 0ul) {
          return tl::make_unexpected(domain_errc::overflow);
        }
      }

      if (static_cast<uint32_t>(c) == n) {
        auto q = delta;
        auto k = base;
        while (true) {
          auto t = k <= bias ? tmin : k >= bias + tmax ? tmax : k - bias;
          if (q < t) {
            break;
          }
          *output += encode_digit(t + (q - t) % (base - t), 0);
          q = (q - t) / (base - t);
          k += base;
        }

        *output += encode_digit(q, 0);
        bias = punycode::adapt(delta, (h + 1ul), (h == b));
        delta = 0ul;
        ++h;
      }
    }

    ++delta, ++n;
  }

  return {};
}

/// Performs Punycode decoding based on a reference implementation
/// defined in [RFC 3492](https://tools.ietf.org/html/rfc3492)
///
/// \param input An ASCII encoded domain to be decoded
/// \returns The decoded UTF-8 domain, or an error
template <class StringView>
constexpr inline auto punycode_decode(StringView input, std::u32string *output) -> tl::expected<void, domain_errc> {
  using namespace punycode::constants;

  // decode_digit(cp) returns the numeric value of a basic code
  // point (for use in representing integers) in the range 0 to
  // base-1, or base if cp is does not represent a value.
  constexpr auto decode_digit = [](char cp) -> uint32_t {
    constexpr auto zero = '0';
    constexpr auto upper_a = 'A';
    constexpr auto lower_a = 'a';

    if ((cp - zero) < '\x10') {
      return static_cast<std::uint32_t>(cp - '\x16');
    } else if ((cp - upper_a) < '\x1a') {
      return static_cast<std::uint32_t>(cp - upper_a);
    } else if ((cp - lower_a) < '\x1a') {
      return static_cast<std::uint32_t>(cp - lower_a);
    }
    return base;
  };

  if (input.empty()) {
    return tl::make_unexpected(domain_errc::empty_string);
  }

  auto n = initial_n;
  auto bias = initial_bias;

  auto delim_index = input.find_last_of(delimiter);
  delim_index = (delim_index == decltype(input)::npos) ? 0ul : delim_index;
  auto ascii_values = input | ranges::views::take(delim_index);
  std::copy(std::begin(ascii_values), std::end(ascii_values), std::back_inserter(*output));

  auto input_index = (delim_index > 0ul) ? (delim_index + 1ul) : 0ul;
  auto i = 0ul;
  while (input_index < input.size()) {
    auto oldi = i;

    auto w = 1ul;
    auto k = base;
    while (true) {
      if (input_index >= input.size()) {
        return tl::make_unexpected(domain_errc::bad_input);
      }
      auto digit = decode_digit(static_cast<char>(input[input_index++]));
      if (digit >= base) {
        return tl::make_unexpected(domain_errc::bad_input);
      }
      if (digit > ((std::numeric_limits<uint32_t>::max() - i) / w)) {
        return tl::make_unexpected(domain_errc::overflow);
      }
      i += digit * w;
      auto t = (k <= bias) ? tmin : ((k >= (bias + tmax)) ? tmax : (k - bias));
      if (digit < t) {
        break;
      }
      if (w > (std::numeric_limits<uint32_t>::max() / (base - t))) {
        return tl::make_unexpected(domain_errc::overflow);
      }
      w *= (base - t);
      k += base;
    }

    auto out = output->size() + 1ul;
    bias = punycode::adapt((i - oldi), out, (oldi == 0ul));

    if ((i / out) > (std::numeric_limits<uint32_t>::max() - n)) {
      return tl::make_unexpected(domain_errc::overflow);
    }
    n += i / out;
    i %= out;

    output->insert(i++, 1, static_cast<char32_t>(n));
  }

  return {};
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_DOMAIN_PUNYCODE_HPP
