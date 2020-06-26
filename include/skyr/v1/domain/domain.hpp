// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_DOMAIN_DOMAIN_HPP
#define SKYR_V1_DOMAIN_DOMAIN_HPP

#include <string>
#include <string_view>
#include <algorithm>
#include <tl/expected.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/iterator.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>
#include <skyr/v1/containers/static_vector.hpp>
#include <skyr/v1/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/v1/unicode/ranges/transforms/u8_transform.hpp>
#include <skyr/v1/unicode/ranges/views/u8_view.hpp>
#include <skyr/v1/domain/errors.hpp>
#include <skyr/v1/domain/idna.hpp>
#include <skyr/v1/domain/punycode.hpp>

/// How many labels can be in a domain?
/// https://www.farsightsecurity.com/blog/txt-record/rrlabel-20171013/
#if !defined(SKYR_DOMAIN_MAX_NUM_LABELS)
#define SKYR_DOMAIN_MAX_NUM_LABELS 32
#endif // !defined(SKYR_DOMAIN_MAX_NUM_LABELS)

namespace skyr {
inline namespace v1 {
inline auto validate_label(std::u32string_view label, [[maybe_unused]] bool use_std3_ascii_rules, bool check_hyphens,
                           [[maybe_unused]] bool check_bidi, [[maybe_unused]] bool check_joiners,
                           bool transitional_processing) -> tl::expected<void, domain_errc> {
  /// https://www.unicode.org/reports/tr46/#Validity_Criteria

  auto first = std::cbegin(label), last = std::cend(label);

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

inline auto domain_to_ascii(
    std::string_view domain_name, bool check_hyphens, bool check_bidi,
    bool check_joiners, bool use_std3_ascii_rules, bool transitional_processing,
    bool verify_dns_length) -> tl::expected<std::string, domain_errc> {
  /// https://www.unicode.org/reports/tr46/#ToASCII

  using namespace std::string_view_literals;

  auto mapped_domain_name =
      unicode::as<std::u32string>(unicode::views::as_u8(domain_name) | unicode::transforms::to_u32);
  if (mapped_domain_name) {
    auto result = idna::map_code_points(std::begin(mapped_domain_name.value()), std::end(mapped_domain_name.value()),
                                        use_std3_ascii_rules, transitional_processing);
    if (result) {
      mapped_domain_name.value().erase(result.value(), mapped_domain_name.value().cend());
    } else {
      return tl::make_unexpected(result.error());
    }
  } else {
    return tl::make_unexpected(domain_errc::encoding_error);
  }

  static constexpr auto to_string_view = [] (auto &&label) {
    return std::u32string_view(std::addressof(*std::cbegin(label)), ranges::distance(label));
  };

  auto labels = static_vector<std::u32string, SKYR_DOMAIN_MAX_NUM_LABELS>{};
  for (auto &&label : mapped_domain_name.value() | ranges::views::split(U'.') | ranges::views::transform(to_string_view)) {
    if (labels.size() == labels.max_size()) {
      return tl::make_unexpected(domain_errc::too_many_labels);
    }

    if ((label.size() >= 4) && (label.substr(0, 4) == U"xn--")) {
      auto decoded = std::u32string{};
      auto result = punycode_decode(label.substr(4), &decoded);
      if (!result) {
        return tl::make_unexpected(result.error());
      }

      auto validated =
          validate_label(decoded, use_std3_ascii_rules, check_hyphens, check_bidi, check_joiners, false);
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

    constexpr static auto is_ascii = [] (std::u32string_view input) noexcept {
      constexpr static auto is_in_ascii_set = [](auto c) { return static_cast<unsigned>(c) <= 0x7eu; };

      auto first = std::cbegin(input), last = std::cend(input);
      return last == std::find_if_not(first, last, is_in_ascii_set);
    };

    labels.emplace_back();
    if (!is_ascii(label)) {
      auto encoded = std::string{};
      auto result = punycode_encode(label, &encoded);
      if (!result) {
        return tl::make_unexpected(result.error());
      }
      ranges::copy(U"xn--"sv, ranges::back_inserter(labels.back()));
      ranges::copy(encoded, ranges::back_inserter(labels.back()));
    }
    else {
      ranges::copy(label, ranges::back_inserter(labels.back()));
    }
  }

  if (mapped_domain_name.value().back() == U'.') {
    labels.emplace_back();
  }

  constexpr auto max_domain_length = 253;
  constexpr auto max_label_length = 63;

  if (verify_dns_length) {
    auto length = mapped_domain_name.value().size();
    if ((length < 1) || (length > max_domain_length)) {
      return tl::make_unexpected(domain_errc::invalid_length);
    }

    for (const auto &label : labels) {
      auto label_length = label.size();
      if ((label_length < 1) || (label_length > max_label_length)) {
        return tl::make_unexpected(domain_errc::invalid_length);
      }
    }
  }

  return labels | ranges::views::join('.') | ranges::to<std::string>();
}


/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \param be_strict Tells the processor to be strict
/// \param validation_error
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(
    std::string_view domain_name,
    bool be_strict,
    bool *validation_error) -> tl::expected<std::string, domain_errc> {
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

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \param be_strict Tells the processor to be strict
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(
    std::string_view domain_name,
    bool be_strict) -> tl::expected<std::string, domain_errc> {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_ascii(domain_name, be_strict, &validation_error);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \param validation_error
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain_name, bool *validation_error) {
  return domain_to_ascii(domain_name, false, validation_error);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain_name) {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_ascii(domain_name, false, &validation_error);
}

/// Converts a Punycode encoded domain to UTF-8
///
/// \param domain_name A Punycode encoded domain
/// \returns A valid UTF-8 encoded domain, or an error
inline auto domain_to_u8(std::string_view domain_name, [[maybe_unused]] bool *validation_error)
-> tl::expected<std::string, domain_errc> {

  static constexpr auto to_string_view = [] (auto &&label) {
    return std::string_view(std::addressof(*std::begin(label)), ranges::distance(label));
  };

  auto labels = static_vector<std::string, SKYR_DOMAIN_MAX_NUM_LABELS>{};
  for (auto &&label : domain_name | ranges::views::split('.') | ranges::views::transform(to_string_view)) {
    if (labels.size() == labels.max_size()) {
      return tl::make_unexpected(domain_errc::too_many_labels);
    }

    labels.emplace_back();
    if (label.substr(0, 4) == "xn--") {
      label.remove_prefix(4);
      auto decoded = std::u32string{};
      auto result = punycode_decode(label, &decoded);
      if (!result) {
        return tl::make_unexpected(result.error());
      }
      auto u8 = decoded | unicode::transforms::to_u8;
      auto first = std::cbegin(u8);
      auto last = std::cend(u8);
      for (auto it = first; it != last; ++it) {
        labels.back().push_back((*it).value());
      }
    }
    else {
      ranges::copy(label, ranges::back_inserter(labels.back()));
    }
  }

  if (domain_name.back() == U'.') {
    labels.emplace_back();
  }

  return labels | ranges::views::join('.') | ranges::to<std::string>();
}

/// Converts a Punycode encoded domain to UTF-8
///
/// \param domain_name A Punycode encoded domain
/// \returns A valid UTF-8 encoded domain, or an error
inline auto domain_to_u8(std::string_view domain_name) -> tl::expected<std::string, domain_errc> {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_u8(domain_name, &validation_error);
}
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_DOMAIN_DOMAIN_HPP
