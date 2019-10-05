// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <cassert>
#include <skyr/unicode/ranges/transforms/byte_transform.hpp>
#include <skyr/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/url/domain.hpp>
#include "algorithms.hpp"
#include "idna_table.hpp"

namespace skyr {
namespace {
class domain_error_category : public std::error_category {
 public:
  [[nodiscard]] const char *name() const noexcept override;
  [[nodiscard]] std::string message(int error) const noexcept override;
};

const char *domain_error_category::name() const noexcept { return "domain"; }

std::string domain_error_category::message(int error) const noexcept {
  switch (static_cast<domain_errc>(error)) {
    case domain_errc::disallowed_code_point:
      return "Disallowed code point";
    case domain_errc::bad_input:
      return "Bad input";
    case domain_errc::overflow:
      return "Overflow";
    case domain_errc::encoding_error:
      return "Encoding error";
    default:
      return "(Unknown error)";
  }
}

const domain_error_category category{};
}  // namespace

std::error_code make_error_code(domain_errc error) noexcept {
  return std::error_code(static_cast<int>(error), category);
}

namespace {
const char32_t base = 36;
const char32_t tmin = 1;
const char32_t tmax = 26;
const char32_t skew = 38;
const char32_t damp = 700;
const char32_t initial_bias = 72;
const char32_t initial_n = 0x80;
const char32_t delimiter = 0x2D;

char32_t decode_digit(char32_t cp) {
  return cp - 48 < 10 ? cp - 22
                      : cp - 65 < 26 ? cp - 65 : cp - 97 < 26 ? cp - 97 : base;
}

char encode_digit(char32_t d, int flag) {
  return static_cast<char>(d + 22 + 75 * (d < 26) - ((flag != 0) << 5));
}

char32_t adapt(char32_t delta, char32_t numpoints, bool firsttime) {
  delta = firsttime ? delta / damp : delta >> 1;
  delta += delta / numpoints;

  auto k = char32_t(0);
  while (delta > ((base - tmin) * tmax) / 2) {
    delta /= base - tmin;
    k += base;
  }
  return k + (base - tmin + 1) * delta / (delta + skew);
}

std::string to_ascii(const std::string &input) { return "xn--" + input; }

inline bool delim(char32_t c) { return c == delimiter; }
}  // namespace

tl::expected<std::string, std::error_code> punycode_encode(
    std::string_view input) {
  auto utf32 = unicode::as<std::u32string>(input | unicode::view::as_u8 |
                                           unicode::transform::to_u32);
  if (!utf32) {
    return tl::make_unexpected(make_error_code(domain_errc::bad_input));
  }
  return punycode_encode(utf32.value());
}

tl::expected<std::string, std::error_code> punycode_encode(
    std::u32string_view input) {
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

tl::expected<std::string, std::error_code> punycode_decode(
    std::string_view input) {
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

namespace {
tl::expected<std::u32string, std::error_code> process(
    std::u32string_view domain_name, bool use_std3_ascii_rules,
    bool check_hyphens, bool check_bidi, bool check_joiners,
    bool transitional_processing) {
  auto result = std::u32string();
  auto error = false;

  auto first = begin(domain_name), last = end(domain_name);
  auto it = first;

  while (it != last) {
    switch (map_idna_status(*it)) {
      case idna_status::disallowed:
        error = true;
        break;
      case idna_status::disallowed_std3_valid:
        if (use_std3_ascii_rules) {
          error = true;
        } else {
          result += *it;
        }
        break;
      case idna_status::disallowed_std3_mapped:
        if (use_std3_ascii_rules) {
          error = true;
        } else {
          result += map_idna_code_point(*it);
        }
        break;
      case idna_status::ignored:
        break;
      case idna_status::mapped:
        result += map_idna_code_point(*it);
        break;
      case idna_status::deviation:
        if (transitional_processing) {
          result += map_idna_code_point(*it);
        } else {
          result += *it;
        }
        break;
      case idna_status::valid:
        result += *it;
        break;
    }

    ++it;
  }

  if (error) {
    return tl::make_unexpected(
        make_error_code(domain_errc::disallowed_code_point));
  }

  return result;
}

tl::expected<std::string, std::error_code> unicode_to_ascii(
    std::u32string_view domain_name, bool check_hyphens, bool check_bidi,
    bool check_joiners, bool use_std3_ascii_rules, bool transitional_processing,
    bool verify_dns_length) {
  auto domain = process(domain_name, use_std3_ascii_rules, check_hyphens,
                        check_bidi, check_joiners, transitional_processing);

  if (!domain) {
    return tl::make_unexpected(domain.error());
  }

  auto labels = split(domain.value(), U'.');

  for (auto &label : labels) {
    if (!is_ascii(label)) {
      auto encoded = punycode_encode(label);
      if (!encoded) {
        return tl::make_unexpected(encoded.error());
      }
      label.assign(begin(encoded.value()), end(encoded.value()));
    }
  }

  if (verify_dns_length) {
    //    auto length = domain.value().size();
    //    if ((length < 1) || (length > 253)) {
    //      return make_unexpected(domain_errc::invalid_length);
    //    }
    //
    //    for (const auto &label : labels) {
    //      auto label_length = label.size();
    //      if ((label_length < 1) || (label_length > 63)) {
    //        return make_unexpected(domain_errc::invalid_length);
    //      }
    //    }
  }

  auto utf32_domain = join(labels, U'.');
  return unicode::as<std::string>(utf32_domain | unicode::transform::to_bytes)
      .or_else([] (auto) -> tl::expected<std::string, std::error_code> {
        return tl::make_unexpected(make_error_code(domain_errc::encoding_error));
      });
}
}  // namespace

tl::expected<std::string, std::error_code> domain_to_ascii(
    std::string_view domain, bool be_strict) {
  auto utf32 = unicode::as<std::u32string>(domain | unicode::view::as_u8 |
                                           unicode::transform::to_u32);
  if (!utf32) {
    return tl::make_unexpected(make_error_code(domain_errc::encoding_error));
  }
  return domain_to_ascii(utf32.value(), be_strict);
}

tl::expected<std::string, std::error_code> domain_to_ascii(
    std::u32string_view domain, bool be_strict) {
  auto result =
      unicode_to_ascii(domain, false, true, true, be_strict, false, be_strict);
  if (!result) {
    // validation error
    return tl::make_unexpected(std::move(result.error()));
  }
  return result;
}
}  // namespace skyr
