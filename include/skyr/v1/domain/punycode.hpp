// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_DOMAIN_PUNYCODE_HPP
#define SKYR_V1_DOMAIN_PUNYCODE_HPP

#include <string>
#include <string_view>
#include <limits>
#include <tl/expected.hpp>
#include <skyr/v1/domain/errors.hpp>


namespace skyr {
inline namespace v1 {
namespace punycode {
namespace constants {
constexpr auto base = 0x24u;
constexpr auto tmin = 0x01u;
constexpr auto tmax = 0x1au;
constexpr auto skew = 0x26u;
constexpr auto damp = 0x2bcu;
constexpr auto initial_bias = 0x48u;
constexpr auto initial_n = 0x80u;
constexpr auto delimiter = 0x2du;
}  // namespace constants

// decode_digit(cp) returns the numeric value of a basic code
// point (for use in representing integers) in the range 0 to
// base-1, or base if cp is does not represent a value.
constexpr inline auto decode_digit(uint32_t cp) -> uint32_t {
  using namespace constants;

  constexpr auto zero = 0x30u;
  constexpr auto upper_a = 0x41u;
  constexpr auto lower_a = 0x61u;

  if ((cp - zero) < 0x10u) {
    return cp - 0x16u;
  }
  else if ((cp - upper_a) < 0x1au) {
    return (cp - upper_a);
  }
  else if ((cp - lower_a) < 0x1au) {
    return (cp - lower_a);
  }
  return base;
}

// encode_digit(d,flag) returns the basic code point whose value
// (when used for representing integers) is d, which needs to be in
// the range 0 to base-1.  The lowercase form is used unless flag is
// nonzero, in which case the uppercase form is used.  The behavior
// is undefined if flag is nonzero and digit d has no uppercase form.
constexpr inline auto encode_digit(uint32_t d, uint32_t flag) -> uint32_t {
  return d + 0x16u + (0x4bu * (d < 0x1au)) - ((flag != 0x0u) << 0x5u);
  //  0..25 map to ASCII a..z or A..Z
  // 26..35 map to ASCII 0..9
}

constexpr inline auto adapt(uint32_t delta, uint32_t numpoints, bool firsttime) {
  using namespace constants;

  delta = firsttime ? delta / damp : delta >> 1u;
  delta += delta / numpoints;

  auto k = 0x00u;
  while (delta > ((base - tmin) * tmax) / 2u) {
    delta /= base - tmin;
    k += base;
  }
  return k + (base - tmin + 1u) * delta / (delta + skew);
}
}  // namespace punycode

/// Performs Punycode encoding based on a reference implementation
/// defined in [RFC 3492](https://tools.ietf.org/html/rfc3492)
///
/// \tparam AsciiString
/// \param input A UTF-32 encoded domain
/// \param output An ascii string on output
/// \returns `void` or an error
inline auto punycode_encode(
    std::u32string_view input,
    std::string *output) -> tl::expected<void, domain_errc> {
  using namespace punycode::constants;

  if (input.empty()) {
    return tl::make_unexpected(domain_errc::empty_string);
  }

  auto n = initial_n;
  auto delta = 0x00u;
  auto bias = initial_bias;

  for (auto c : input) {
    if (static_cast<uint32_t>(c) < 0x80u) {
      *output += static_cast<char>(c);
    }
  }

  auto h = static_cast<uint32_t>(output->size());
  auto b = static_cast<uint32_t>(output->size());

  if (b > 0x00u) {
    *output += static_cast<char>(delimiter);
  }

  while (h < input.size()) {
    auto m = std::numeric_limits<uint32_t>::max();
    for (auto c : input) {
      if ((static_cast<uint32_t>(c) >= n) && (static_cast<uint32_t>(c) < m)) {
        m = c;
      }
    }

    if ((m - n) > ((std::numeric_limits<uint32_t>::max() - delta) / (h + 1u))) {
      return tl::make_unexpected(domain_errc::overflow);
    }
    delta += (m - n) * (h + 1u);
    n = m;

    for (auto c : input) {
      if (static_cast<uint32_t>(c) < n) {
        if (++delta == 0u) {
          return tl::make_unexpected(domain_errc::overflow);
        }
      }

      if (static_cast<uint32_t>(c) == n) {
        auto q = delta;
        auto k = uint32_t(base);
        while (true) {
          auto t = k <= bias ? tmin : k >= bias + tmax ? tmax : k - bias;
          if (q < t) {
            break;
          }
          *output += static_cast<char>(punycode::encode_digit(t + (q - t) % (base - t), 0));
          q = (q - t) / (base - t);
          k += base;
        }

        *output += static_cast<char>(punycode::encode_digit(q, 0));
        bias = punycode::adapt(delta, (h + 1), (h == b));
        delta = 0;
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
template <class charT>
inline auto punycode_decode(
    std::basic_string_view<charT> input,
    std::u32string *output) -> tl::expected<void, domain_errc> {
  using namespace punycode::constants;

  if (input.empty()) {
    return tl::make_unexpected(domain_errc::empty_string);
  }

  auto n = initial_n;
  auto bias = initial_bias;

  auto basic = 0x0u;
  for (auto j = 0u; j < input.size(); ++j) {
    if (input[j] == delimiter) {
      basic = j;
    }
  }

  for (auto j = 0u; j < basic; ++j) {
    *output += input[j];
  }

  auto in = (basic > 0x0u) ? (basic + 0x01u) : 0x00u;
  auto i = 0x00u;
  while (in < input.size()) {
    auto oldi = i;

    auto w = 0x01u;
    auto k = base;
    while (true) {
      if (in >= input.size()) {
        return tl::make_unexpected(domain_errc::bad_input);
      }
      auto digit = punycode::decode_digit(input[in++]);
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

    auto out = static_cast<std::uint32_t>(output->size()) + 1U;
    bias = punycode::adapt((i - oldi), out, (oldi == 0U));

    if ((i / out) > (std::numeric_limits<uint32_t>::max() - n)) {
      return tl::make_unexpected(domain_errc::overflow);
    }
    n += i / out;
    i %= out;

    output->insert(i++, 1, static_cast<char32_t>(n));
  }

  return {};
}
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_DOMAIN_PUNYCODE_HPP
