// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <cassert>
#include <skyr/v1/domain/errors.hpp>
#include <skyr/v1/unicode/ranges/views//u8_view.hpp>
#include <skyr/v1/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/v1/unicode/ranges/transforms/u8_transform.hpp>
#include <v1/domain/punycode.hpp>

namespace skyr {
inline namespace v1 {
namespace {
using namespace std::string_literals;
using namespace std::string_view_literals;

constexpr auto base = 0x24u;
constexpr auto tmin = 0x01u;
constexpr auto tmax = 0x1au;
constexpr auto skew = 0x26u;
constexpr auto damp = 0x2bcu;
constexpr auto initial_bias = 0x48u;
constexpr auto initial_n = 0x80u;
constexpr auto delimiter = 0x2du;

// decode_digit(cp) returns the numeric value of a basic code
// point (for use in representing integers) in the range 0 to
// base-1, or base if cp is does not represent a value.
constexpr inline auto decode_digit(uint32_t cp) -> uint32_t {
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

auto adapt(uint32_t delta, uint32_t numpoints, bool firsttime) {
  delta = firsttime ? delta / damp : delta >> 1u;
  delta += delta / numpoints;

  auto k = 0x00u;
  while (delta > ((base - tmin) * tmax) / 2u) {
    delta /= base - tmin;
    k += base;
  }
  return k + (base - tmin + 1u) * delta / (delta + skew);
}
}  // namespace

auto punycode_encode(
    std::u32string_view input) -> tl::expected<std::string, domain_errc> {
  if (input.empty()) {
    return tl::make_unexpected(domain_errc::empty_string);
  }

  auto result = std::string{};
  result.reserve(256);

  auto n = initial_n;
  auto delta = 0x00u;
  auto bias = initial_bias;

  for (auto c : input) {
    if (static_cast<uint32_t>(c) < 0x80u) {
      result += static_cast<char>(c);
    }
  }

  auto h = static_cast<char32_t>(result.size());
  auto b = static_cast<char32_t>(result.size());

  if (b > 0x00u) {
    result += static_cast<char>(delimiter);
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
          result += static_cast<char>(encode_digit(t + (q - t) % (base - t), 0));
          q = (q - t) / (base - t);
          k += base;
        }

        result += static_cast<char>(encode_digit(q, 0));
        bias = adapt(delta, (h + 1), (h == b));
        delta = 0;
        ++h;
      }
    }

    ++delta, ++n;
  }

  return result;
}

auto punycode_decode(
    std::string_view input) -> tl::expected<std::string, domain_errc> {
  if (input.empty()) {
    return tl::make_unexpected(domain_errc::empty_string);
  }

  auto result = std::u32string();
  result.reserve(256);

  auto n = initial_n;
  auto bias = initial_bias;

  auto basic = 0x0u;
  for (auto j = 0u; j < input.size(); ++j) {
    if (input[j] == delimiter) {
      basic = j;
    }
  }

  for (auto j = 0u; j < basic; ++j) {
    result += input[j];
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
      auto digit = decode_digit(input[in++]);
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

    auto out = static_cast<std::uint32_t>(result.size()) + 1U;
    bias = adapt((i - oldi), out, (oldi == 0U));

    if ((i / out) > (std::numeric_limits<uint32_t>::max() - n)) {
      return tl::make_unexpected(domain_errc::overflow);
    }
    n += i / out;
    i %= out;

    result.insert(i++, 1, static_cast<char32_t>(n));
  }

  auto u8_result = unicode::as<std::string>(result | unicode::transforms::to_u8);
  if (!u8_result) {
    return tl::make_unexpected(domain_errc::bad_input);
  }
  return u8_result.value();
}

auto punycode_decode(
    std::u32string_view input) -> tl::expected<std::u32string, domain_errc> {
  if (input.empty()) {
    return tl::make_unexpected(domain_errc::empty_string);
  }

  auto u8input = unicode::as<std::string>(input | unicode::transforms::to_u8).value();
  return punycode_decode(std::string_view(u8input))
      .and_then([] (auto &&output) -> tl::expected<std::u32string, domain_errc> {
        return unicode::as<std::u32string>(unicode::views::as_u8(output) | unicode::transforms::to_u32).value();
      });
}

auto punycode_encode(
    std::string_view input) -> tl::expected<std::string, domain_errc> {
  if (input.empty()) {
    return tl::make_unexpected(domain_errc::empty_string);
  }

  auto utf32 = unicode::as<std::u32string>(unicode::views::as_u8(input) | unicode::transforms::to_u32);
  if (!utf32) {
    return tl::make_unexpected(domain_errc::bad_input);
  }
  return punycode_encode(utf32.value());
}
}  // namespace v1
}  // namespace skyr
