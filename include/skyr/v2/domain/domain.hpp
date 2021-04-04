// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_DOMAIN_DOMAIN_HPP
#define SKYR_V2_DOMAIN_DOMAIN_HPP

#include <string>
#include <string_view>
#include <algorithm>
#include <iterator>
#include <tl/expected.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/action/erase.hpp>
#include <range/v3/iterator.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>
#include <skyr/v2/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/v2/unicode/ranges/transforms/u8_transform.hpp>
#include <skyr/v2/unicode/ranges/views/u8_view.hpp>
#include <skyr/v2/domain/errors.hpp>
#include <skyr/v2/domain/idna.hpp>
#include <skyr/v2/domain/punycode.hpp>

namespace skyr::inline v2 {
constexpr inline auto validate_label(std::u32string_view label, [[maybe_unused]] bool use_std3_ascii_rules,
                                     bool check_hyphens, [[maybe_unused]] bool check_bidi,
                                     [[maybe_unused]] bool check_joiners, bool transitional_processing)
    -> tl::expected<void, domain_errc> {
  /// https://www.unicode.org/reports/tr46/#Validity_Criteria;

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
    constexpr auto is_valid = [](auto cp) {
      auto status = idna::code_point_status(cp);
      return (cp <= U'\x7e') || (status == idna::idna_status::valid);
    };

    if (std::cend(label) != std::find_if_not(std::cbegin(label), std::cend(label), is_valid)) {
      return tl::make_unexpected(domain_errc::bad_input);
    }
  } else {
    constexpr auto is_valid_or_deviation = [](auto cp) {
      auto status = idna::code_point_status(cp);
      return (cp <= U'\x7e') || (status == idna::idna_status::valid) || (status == idna::idna_status::deviation);
    };

    if (std::cend(label) != std::find_if_not(std::cbegin(label), std::cend(label), is_valid_or_deviation)) {
      return tl::make_unexpected(domain_errc::bad_input);
    }
  }

  return {};
}

///
struct domain_to_ascii_context {
  /// Stores the domain as UTF-32
  std::u32string domain_name;

  /// Parameters
  std::string *ascii_domain;
  bool check_hyphens;
  bool check_bidi;
  bool check_joiners;
  bool use_std3_ascii_rules;
  bool transitional_processing;
  bool verify_dns_length;

  // These are intermediate buffers
  std::vector<std::u32string> labels;
  std::string punycode_encoded;
  std::u32string punycode_decoded;
};

///
/// \param domain_name
/// \param ascii_domain
/// \param check_hyphens
/// \param check_bidi
/// \param check_joiners
/// \param use_std3_ascii_rules
/// \param transitional_processing
/// \param verify_dns_length
/// \return
inline auto create_domain_to_ascii_context(std::string_view domain_name, std::string *ascii_domain, bool check_hyphens,
                                           bool check_bidi, bool check_joiners, bool use_std3_ascii_rules,
                                           bool transitional_processing, bool verify_dns_length)
    -> tl::expected<domain_to_ascii_context, domain_errc> {
  auto u32domain_name = unicode::as<std::u32string>(unicode::views::as_u8(domain_name) | unicode::transforms::to_u32);
  if (u32domain_name) {
    return domain_to_ascii_context{u32domain_name.value(),
                                   ascii_domain,
                                   check_hyphens,
                                   check_bidi,
                                   check_joiners,
                                   use_std3_ascii_rules,
                                   transitional_processing,
                                   verify_dns_length,
                                   {},
                                   {},
                                   {}};
  } else {
    return tl::make_unexpected(domain_errc::encoding_error);
  }
}

///
/// \param context
/// \return
inline auto domain_to_ascii_impl(domain_to_ascii_context &&context) -> tl::expected<void, domain_errc> {
  /// https://www.unicode.org/reports/tr46/#ToASCII

  constexpr auto map_domain_name =
      [](domain_to_ascii_context &&ctx) -> tl::expected<domain_to_ascii_context, domain_errc> {
    auto result = idna::map_code_points(ctx.domain_name, ctx.use_std3_ascii_rules, ctx.transitional_processing);
    if (result) {
      ctx.domain_name.erase(result.value(), std::cend(ctx.domain_name));
      return std::move(ctx);
    } else {
      return tl::make_unexpected(result.error());
    }
  };

  constexpr auto process_labels = [](auto &&ctx) -> tl::expected<std::decay_t<decltype(ctx)>, domain_errc> {
    using namespace std::string_view_literals;

    constexpr auto to_string_view = [](auto &&label) {
      auto size = ranges::distance(label);
      return std::u32string_view(std::addressof(*std::cbegin(label)), size);
    };

    for (auto &&label : ctx.domain_name | ranges::views::split(U'.') | ranges::views::transform(to_string_view)) {
      if ((label.size() >= 4) && (label.substr(0, 4) == U"xn--")) {
        ctx.punycode_decoded.clear();
        auto decoded = punycode_decode(label.substr(4), &ctx.punycode_decoded);
        if (!decoded) {
          return tl::make_unexpected(decoded.error());
        }

        auto validated = validate_label(ctx.punycode_decoded, ctx.use_std3_ascii_rules, ctx.check_hyphens,
                                        ctx.check_bidi, ctx.check_joiners, false);
        if (!validated) {
          return tl::make_unexpected(validated.error());
        }
      } else {
        auto validated = validate_label(label, ctx.use_std3_ascii_rules, ctx.check_hyphens, ctx.check_bidi,
                                        ctx.check_joiners, ctx.transitional_processing);
        if (!validated) {
          return tl::make_unexpected(validated.error());
        }
      }

      constexpr auto is_ascii = [](std::u32string_view input) noexcept {
        constexpr auto is_in_ascii_set = [](auto c) { return c <= U'\x7e'; };

        return ranges::cend(input) == ranges::find_if_not(input, is_in_ascii_set);
      };

      ctx.labels.emplace_back();
      if (!is_ascii(label)) {
        ctx.punycode_encoded.clear();
        auto result = punycode_encode(label, &ctx.punycode_encoded);
        if (!result) {
          return tl::make_unexpected(result.error());
        }
        ranges::copy(U"xn--"sv, ranges::back_inserter(ctx.labels.back()));
        ranges::copy(ctx.punycode_encoded, ranges::back_inserter(ctx.labels.back()));
      } else {
        ranges::copy(label, ranges::back_inserter(ctx.labels.back()));
      }
    }

    if (ctx.domain_name.back() == U'.') {
      ctx.labels.emplace_back();
    }

    return std::move(ctx);
  };

  constexpr auto check_length =
      [](auto &&ctx) -> tl::expected<std::decay_t<decltype(ctx)>, domain_errc> {
    constexpr auto max_domain_length = 253;
    constexpr auto max_label_length = 63;

    if (ctx.verify_dns_length) {
      auto length = ctx.domain_name.size();
      if ((length < 1) || (length > max_domain_length)) {
        return tl::make_unexpected(domain_errc::invalid_length);
      }

      for (const auto &label : ctx.labels) {
        auto label_length = label.size();
        if ((label_length < 1) || (label_length > max_label_length)) {
          return tl::make_unexpected(domain_errc::invalid_length);
        }
      }
    }

    return std::move(ctx);
  };

  constexpr auto copy_to_output = [](domain_to_ascii_context &&ctx) -> tl::expected<void, domain_errc> {
    ranges::copy(ctx.labels | ranges::views::join('.'), ranges::back_inserter(*ctx.ascii_domain));
    return {};
  };

  return map_domain_name(std::move(context)).and_then(process_labels).and_then(check_length).and_then(copy_to_output);
}

///
/// \param domain_name
/// \param ascii_domain
/// \param check_hyphens
/// \param check_bidi
/// \param check_joiners
/// \param use_std3_ascii_rules
/// \param transitional_processing
/// \param verify_dns_length
/// \return
inline auto domain_to_ascii(std::string_view domain_name, std::string *ascii_domain, bool check_hyphens,
                            bool check_bidi, bool check_joiners, bool use_std3_ascii_rules,
                            bool transitional_processing, bool verify_dns_length) -> tl::expected<void, domain_errc> {
  return create_domain_to_ascii_context(domain_name, ascii_domain, check_hyphens, check_bidi, check_joiners,
                                        use_std3_ascii_rules, transitional_processing, verify_dns_length)
      .and_then(domain_to_ascii_impl);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \param be_strict Tells the processor to be strict
/// \param validation_error
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain_name, std::string *ascii_domain, bool be_strict,
                            bool *validation_error) -> tl::expected<void, domain_errc> {
  auto result = domain_to_ascii(domain_name, ascii_domain, false, true, true, be_strict, false, be_strict);
  if (!result) {
    *validation_error |= true;
  } else if (ascii_domain->empty()) {
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
inline auto domain_to_ascii(std::string_view domain_name, std::string *ascii_domain, bool be_strict)
    -> tl::expected<void, domain_errc> {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_ascii(domain_name, ascii_domain, be_strict, &validation_error);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \param validation_error
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain_name, std::string *ascii_domain, bool *validation_error) {
  return domain_to_ascii(domain_name, ascii_domain, false, validation_error);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain_name, std::string *ascii_domain) {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_ascii(domain_name, ascii_domain, false, &validation_error);
}

struct domain_to_u8_context {
  std::string_view domain_name;

  /// Parameters
  std::string *u8_domain;

  std::vector<std::string> labels;

  /// This is used as an intermediate buffer
  std::u32string punycode_decoded;
};

///
/// \param context
/// \return
inline auto domain_to_u8_impl(domain_to_u8_context &&context) -> tl::expected<void, domain_errc> {
  static constexpr auto to_string_view = [](auto &&label) {
    return std::string_view(std::addressof(*std::begin(label)), ranges::distance(label));
  };

  for (auto &&label : context.domain_name | ranges::views::split('.') | ranges::views::transform(to_string_view)) {
    context.labels.emplace_back();
    if (label.substr(0, 4) == "xn--") {
      label.remove_prefix(4);
      context.punycode_decoded.clear();
      auto result = punycode_decode(label, &context.punycode_decoded);
      if (!result) {
        return tl::make_unexpected(result.error());
      }
      auto u8 = context.punycode_decoded | unicode::transforms::to_u8;
      auto first = std::cbegin(u8);
      auto last = std::cend(u8);
      for (auto it = first; it != last; ++it) {
        context.labels.back().push_back((*it).value());
      }
    } else {
      ranges::copy(label, ranges::back_inserter(context.labels.back()));
    }
  }

  if (context.domain_name.back() == U'.') {
    context.labels.emplace_back();
  }

  ranges::copy(context.labels | ranges::views::join('.'), ranges::back_inserter(*context.u8_domain));

  return {};
}

/// Converts a Punycode encoded domain to UTF-8
///
/// \param domain_name A Punycode encoded domain
/// \returns A valid UTF-8 encoded domain, or an error
inline auto domain_to_u8(std::string_view domain_name, std::string *u8_domain, [[maybe_unused]] bool *validation_error)
    -> tl::expected<void, domain_errc> {
  auto context = domain_to_u8_context{domain_name, u8_domain, {}, {}};
  return domain_to_u8_impl(std::move(context));
}

/// Converts a Punycode encoded domain to UTF-8
///
/// \param domain_name A Punycode encoded domain
/// \returns A valid UTF-8 encoded domain, or an error
inline auto domain_to_u8(std::string_view domain_name, std::string *u8_domain) -> tl::expected<void, domain_errc> {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_u8(domain_name, u8_domain, &validation_error);
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_DOMAIN_DOMAIN_HPP
