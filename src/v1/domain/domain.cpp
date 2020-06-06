// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <skyr/v1/domain/domain.hpp>
#include <skyr/v1/domain/errors.hpp>
#include <skyr/v1/ranges/string_element_range.hpp>
#include <skyr/v1/string/ascii.hpp>
#include <skyr/v1/string/join.hpp>
#include <skyr/v1/string/split.hpp>
#include <skyr/v1/unicode/ranges/transforms/u32_transform.hpp>
#include <v1/domain/punycode.hpp>
#include "idna.hpp"

namespace skyr {
inline namespace v1 {
namespace {
auto map_code_points(
    std::u32string_view domain_name, bool use_std3_ascii_rules,
    bool transitional_processing)
    -> tl::expected<std::u32string, domain_errc> {
  auto result = std::u32string();
  auto error = false;

  auto first = begin(domain_name), last = end(domain_name);
  auto it = first;

  while (it != last) {
    switch (domain::map_idna_status(*it)) {
      case domain::idna_status::disallowed:
        error = true;
        break;
      case domain::idna_status::disallowed_std3_valid:
        if (use_std3_ascii_rules) {
          error = true;
        } else {
          result += *it;
        }
        break;
      case domain::idna_status::disallowed_std3_mapped:
        if (use_std3_ascii_rules) {
          error = true;
        } else {
          result += domain::map_idna_code_point(*it);
        }
        break;
      case domain::idna_status::ignored:
        break;
      case domain::idna_status::mapped:
        result += domain::map_idna_code_point(*it);
        break;
      case domain::idna_status::deviation:
        if (transitional_processing) {
          result += domain::map_idna_code_point(*it);
        } else {
          result += *it;
        }
        break;
      case domain::idna_status::valid:
        result += *it;
        break;
    }

    ++it;
  }

  if (error) {
    return tl::make_unexpected(domain_errc::disallowed_code_point);
  }

  return result;
}

auto unicode_to_ascii(
    std::u32string_view domain_name, bool check_hyphens, [[maybe_unused]] bool check_bidi,
    bool check_joiners, bool use_std3_ascii_rules, bool transitional_processing,
    bool verify_dns_length) -> tl::expected<std::string, domain_errc> {
  constexpr static auto is_contextj = [] (auto cp) {
    return (cp == U'\x200c') || (cp == U'\x200d');
  };

  for (auto label : split(std::u32string_view(domain_name), U"\x002e\xff0e\x3002\0xff61")) {
    if (check_hyphens) {
      if ((label.size() >= 4) && (label.substr(2, 4) == U"--")) {
        return tl::make_unexpected(domain_errc::bad_input);
      }

      if ((label.front() == U'-') || (label.back() == U'-')) {
        return tl::make_unexpected(domain_errc::bad_input);
      }
    }

    if (check_joiners) {
      auto first = begin(label), last = end(label);
      auto it = std::find_if(first, last, is_contextj);
      if (it != last) {
        return tl::make_unexpected(domain_errc::bad_input);
      }
    }
  }

  auto domain = map_code_points(domain_name, use_std3_ascii_rules, transitional_processing);

  if (!domain) {
    return tl::make_unexpected(domain.error());
  }

  auto labels = std::vector<std::string>{};
  for (auto label : split(std::u32string_view(domain.value()), U".")) {
    if (!is_ascii(label)) {
      auto encoded = punycode_encode(label);
      if (!encoded) {
        return tl::make_unexpected(encoded.error());
      }
      labels.emplace_back("xn--" + encoded.value());
    }
    else {
      labels.emplace_back(begin(label), end(label));
    }
  }

  if (verify_dns_length) {
    auto length = domain.value().size();
    if ((length < 1) || (length > 253)) {
      return tl::make_unexpected(domain_errc::invalid_length);
    }

    for (const auto &label : labels) {
      auto label_length = label.size();
      if ((label_length < 1) || (label_length > 63)) {
        return tl::make_unexpected(domain_errc::invalid_length);
      }
    }
  }

  return join(labels, '.');
}

auto domain_to_ascii(
    std::u32string_view domain, bool be_strict, bool *validation_error) -> tl::expected<std::string, domain_errc> {
  auto result =
      unicode_to_ascii(domain, false, true, true, be_strict, false, be_strict);
  if (!result) {
    *validation_error |= true;
    return tl::make_unexpected(result.error());
  }
  return result;
}
}  // namespace

auto domain_to_ascii(
    std::string_view domain, bool be_strict, bool *validation_error) -> tl::expected<std::string, domain_errc> {
  auto utf32 = unicode::as<std::u32string>(unicode::views::as_u8(domain) | unicode::transforms::to_u32);
  if (!utf32) {
    return tl::make_unexpected(domain_errc::encoding_error);
  }
  return domain_to_ascii(utf32.value(), be_strict, validation_error);
}

auto domain_to_unicode(std::string_view ascii) -> tl::expected<std::string, domain_errc> {
  auto labels = std::vector<std::string>{};
  for (auto label : split(ascii, ".")) {
    if (label.substr(0, 4) == "xn--") {
      label.remove_prefix(4);
      auto encoded = punycode_decode(label);
      if (!encoded) {
        return tl::make_unexpected(encoded.error());
      }
      labels.emplace_back(encoded.value());
    }
    else {
      labels.emplace_back(begin(label), end(label));
    }
  }
  return join(labels, '.');
}
}  // namespace v1
}  // namespace skyr
