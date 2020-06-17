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
    switch (idna::code_point_status(*it)) {
      case idna::idna_status::disallowed:
        error = true;
        break;
      case idna::idna_status::disallowed_std3_valid:
        if (use_std3_ascii_rules) {
          error = true;
        } else {
          result += *it;
        }
        break;
      case idna::idna_status::disallowed_std3_mapped:
        if (use_std3_ascii_rules) {
          error = true;
        } else {
          result += idna::map_code_point(*it);
        }
        break;
      case idna::idna_status::ignored:
        break;
      case idna::idna_status::mapped:
        result += idna::map_code_point(*it);
        break;
      case idna::idna_status::deviation:
        if (transitional_processing) {
          result += idna::map_code_point(*it);
        } else {
          result += *it;
        }
        break;
      case idna::idna_status::valid:
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

auto validate_label(std::u32string_view label, [[maybe_unused]] bool use_std3_ascii_rules, bool check_hyphens,
                    [[maybe_unused]] bool check_bidi, [[maybe_unused]] bool check_joiners, bool transitional_processing)
    -> tl::expected<void, domain_errc> {
  /// https://www.unicode.org/reports/tr46/#Validity_Criteria

  auto first = begin(label), last = end(label);

  if (check_hyphens) {
    /// Criterion 2
    if ((label.size() >= 4) && (label.substr(2, 4) == U"--")) {
      return tl::make_unexpected(domain_errc::bad_input);
    }

    /// Criterion 3
    if ((label.front() == U'-') || (label.back() == U'-')) {
      return tl::make_unexpected(domain_errc::bad_input);
    }
  }

  /// Criterion 6
  if (transitional_processing) {
    static constexpr auto is_valid = [](auto cp) {
      auto status = idna::code_point_status(cp);
      return (cp <= U'\x7e') || (status == idna::idna_status::valid);
    };

    auto it = std::find_if_not(first, last, is_valid);
    if (it != last) {
      return tl::make_unexpected(domain_errc::bad_input);
    }
  }
  else {
    static constexpr auto is_valid_or_deviation = [](auto cp) {
      auto status = idna::code_point_status(cp);
      return (cp <= U'\x7e') || (status == idna::idna_status::valid) || (status == idna::idna_status::deviation);
    };

    auto it = std::find_if_not(first, last, is_valid_or_deviation);
    if (it != last) {
      return tl::make_unexpected(domain_errc::bad_input);
    }
  }

  return {};
}

auto idna_process(std::u32string_view domain_name, bool use_std3_ascii_rules, bool check_hyphens,
                  bool check_bidi, bool check_joiners, bool transitional_processing)
    -> tl::expected<std::u32string, domain_errc> {
  using namespace std::string_view_literals;

  auto result = map_code_points(domain_name, use_std3_ascii_rules, transitional_processing);
  if (result) {
    for (auto label : split(std::u32string_view(result.value()), U"."sv)) {
      if ((label.size() >= 4) && (label.substr(0, 4) == U"xn--")) {
        auto decoded = punycode_decode(label.substr(4));
        if (!decoded) {
          return tl::make_unexpected(decoded.error());
        }

        auto validated =
            validate_label(decoded.value(), use_std3_ascii_rules, check_hyphens, check_bidi, check_joiners, false);
        if (!validated) {
          return tl::make_unexpected(validated.error());
        }
      } else {
        auto validated = validate_label(label, use_std3_ascii_rules, check_hyphens, check_bidi, check_joiners,
                                        transitional_processing);
        if (!validated) {
          return tl::make_unexpected(validated.error());
        }
      }
    }
  }
  return result;
}

auto domain_to_ascii(
    std::string_view domain_name, bool check_hyphens, bool check_bidi,
    bool check_joiners, bool use_std3_ascii_rules, bool transitional_processing,
    bool verify_dns_length) -> tl::expected<std::string, domain_errc> {
  /// https://www.unicode.org/reports/tr46/#ToASCII

  auto utf32 = unicode::as<std::u32string>(unicode::views::as_u8(domain_name) | unicode::transforms::to_u32);
  if (!utf32) {
    return tl::make_unexpected(domain_errc::encoding_error);
  }

  auto domain = idna_process(
      utf32.value(), use_std3_ascii_rules, check_hyphens, check_bidi, check_joiners, transitional_processing);
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
}  // namespace

auto domain_to_ascii(
    std::string_view domain_name, bool be_strict, bool *validation_error) -> tl::expected<std::string, domain_errc> {
  auto result = domain_to_ascii(domain_name, false, true, true, be_strict, false, be_strict);
  if (!result) {
    *validation_error |= true;
    return tl::make_unexpected(result.error());
  }
  else if (result.value().empty()) {
    *validation_error |= true;
    return tl::make_unexpected(domain_errc::empty_string);
  }
  return result;
}

auto domain_to_u8(std::string_view domain_name, [[maybe_unused]] bool *validation_error)
    -> tl::expected<std::string, domain_errc> {
  auto labels = std::vector<std::string>{};
  for (auto label : split(domain_name, ".")) {
    if (label.substr(0, 4) == "xn--") {
      label.remove_prefix(4);
      auto decoded = punycode_decode(label);
      if (!decoded) {
        return tl::make_unexpected(decoded.error());
      }
      labels.emplace_back(decoded.value());
    }
    else {
      labels.emplace_back(begin(label), end(label));
    }
  }
  return join(labels, '.');
}
}  // namespace v1
}  // namespace skyr
