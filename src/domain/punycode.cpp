// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <cassert>
#include <skyr/unicode/ranges/transforms/byte_transform.hpp>
#include <skyr/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/domain/punycode.hpp>
#include <skyr/domain/errors.hpp>

namespace skyr {
inline namespace v1 {
namespace {
using namespace std::string_literals;
using namespace std::string_view_literals;

const char32_t base = 36;
const char32_t tmin = 1;
const char32_t tmax = 26;
const char32_t skew = 38;
const char32_t damp = 700;
const char32_t initial_bias = 72;
const char32_t initial_n = 0x80;
const char32_t delimiter = 0x2D;

auto decode_digit(char32_t cp) {
  return ((cp - 48) < 10) ?
         (cp - 22) :
         ((cp - 65) < 26) ? (cp - 65) : ((cp - 97) < 26) ? (cp - 97) : base;
}

auto encode_digit(char32_t d, unsigned int flag) {
  return static_cast<char>(d + 22 + 75 * (d < 26) - ((flag != 0u) << 5u));
}

auto adapt(char32_t delta, char32_t numpoints, bool firsttime) {
  delta = firsttime ? delta / damp : delta >> 1u;
  delta += delta / numpoints;

  auto k = U'\x00';
  while (delta > ((base - tmin) * tmax) / 2) {
    delta /= base - tmin;
    k += base;
  }
  return k + (base - tmin + 1) * delta / (delta + skew);
}

inline auto to_ascii(const std::string &input) { return "xn--"s + input; }

inline auto delim(char32_t c) { return c == delimiter; }
}  // namespace

auto punycode_encode(
    std::string_view input) -> tl::expected<std::string, std::error_code> {
  auto utf32 = unicode::as<std::u32string>(unicode::view::as_u8(input) | unicode::transform::to_u32);
  if (!utf32) {
    return tl::make_unexpected(make_error_code(domain_errc::bad_input));
  }
  return punycode_encode(utf32.value());
}

auto punycode_encode(
    std::u32string_view input) -> tl::expected<std::string, std::error_code> {
  auto result = std::string{};
  result.reserve(256);

  auto n = initial_n;
  auto delta = 0U;
  auto bias = initial_bias;

  for (auto c : input) {
    if (c < 0x80) {
      result += static_cast<char>(c);
    }
  }

  auto h = static_cast<char32_t>(result.size());
  auto b = static_cast<char32_t>(result.size());

  if (b > 0) {
    result += delimiter;
  }

  while (h < input.size()) {
    auto m = std::numeric_limits<char32_t>::max();
    for (auto c : input) {
      if ((c >= n) && (c < m)) {
        m = c;
      }
    }

    if ((m - n) > ((std::numeric_limits<char32_t>::max() - delta) / (h + 1))) {
      return tl::make_unexpected(make_error_code(domain_errc::overflow));
    }
    delta += (m - n) * (h + 1);
    n = m;

    for (auto c : input) {
      if (c < n) {
        if (++delta == 0) {
          return tl::make_unexpected(make_error_code(domain_errc::overflow));
        }
      }

      if (c == n) {
        auto q = delta;
        auto k = uint32_t(base);
        while (true) {
          auto t = k <= bias ? tmin : k >= bias + tmax ? tmax : k - bias;
          if (q < t) {
            break;
          }
          result += encode_digit(t + (q - t) % (base - t), 0);
          q = (q - t) / (base - t);
          k += base;
        }

        result += encode_digit(q, 0);
        bias = adapt(delta, (h + 1), (h == b));
        delta = 0;
        ++h;
      }
    }

    ++delta, ++n;
  }

  return to_ascii(result);
}

auto punycode_decode(
    std::string_view input) -> tl::expected<std::string, std::error_code> {
  auto result = std::u32string();
  result.reserve(256);

  if (input.substr(0, 4).compare("xn--") == 0) {
    input.remove_prefix(4);
  } else {
    return std::string(input);
  }

  auto n = initial_n;
  auto bias = initial_bias;

  auto basic = 0U;
  for (auto j = 0U; j < input.size(); ++j) {
    if (delim(input[j])) {
      basic = j;
    }
  }

  for (auto j = 0U; j < basic; ++j) {
    result += input[j];
  }

  auto in = static_cast<char32_t>((basic > 0U) ? (basic + 1U) : 0U);
  auto i = static_cast<char32_t>(0U);
  while (in < input.size()) {
    auto oldi = i;

    auto w = static_cast<char32_t>(1U);
    auto k = base;
    while (true) {
      if (in >= input.size()) {
        return tl::make_unexpected(make_error_code(domain_errc::bad_input));
      }
      auto digit = decode_digit(input[in++]);
      if (digit >= base) {
        return tl::make_unexpected(make_error_code(domain_errc::bad_input));
      }
      if (digit > ((std::numeric_limits<char32_t>::max() - i) / w)) {
        return tl::make_unexpected(make_error_code(domain_errc::overflow));
      }
      i += digit * w;
      auto t = (k <= bias) ? tmin : ((k >= (bias + tmax)) ? tmax : (k - bias));
      if (digit < t) {
        break;
      }
      if (w > (std::numeric_limits<char32_t>::max() / (base - t))) {
        return tl::make_unexpected(make_error_code(domain_errc::overflow));
      }
      w *= (base - t);
      k += base;
    }

    auto out = static_cast<char32_t>(result.size() + 1U);
    bias = adapt((i - oldi), out, (oldi == 0U));

    if ((i / out) > (std::numeric_limits<char32_t>::max() - n)) {
      return tl::make_unexpected(make_error_code(domain_errc::overflow));
    }
    n += i / out;
    i %= out;

    result.insert(i++, 1, n);
  }

  return unicode::as<std::string>(result | unicode::transform::to_bytes)
      .or_else([](auto) -> tl::expected<std::string, std::error_code> {
        return tl::make_unexpected(make_error_code(domain_errc::bad_input));
      });
}
}  // namespace v1
}  // namespace skyr
