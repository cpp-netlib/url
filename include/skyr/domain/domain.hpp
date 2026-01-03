// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_DOMAIN_DOMAIN_HPP
#define SKYR_DOMAIN_DOMAIN_HPP

#include <algorithm>
#include <expected>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>

#include <skyr/domain/errors.hpp>
#include <skyr/domain/idna.hpp>
#include <skyr/domain/punycode.hpp>
#include <skyr/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/unicode/ranges/transforms/u8_transform.hpp>
#include <skyr/unicode/ranges/views/u8_view.hpp>

namespace skyr {
constexpr inline auto validate_label(std::u32string_view label, [[maybe_unused]] bool use_std3_ascii_rules,
                                     bool check_hyphens, [[maybe_unused]] bool check_bidi,
                                     [[maybe_unused]] bool check_joiners, bool transitional_processing)
    -> std::expected<void, domain_errc> {
  /// https://www.unicode.org/reports/tr46/#Validity_Criteria;

  if (check_hyphens) {
    /// Criterion 2
    if ((label.size() >= 4) && (label.substr(2, 4) == U"--")) {
      return std::unexpected(domain_errc::bad_input);
    }

    /// Criterion 3
    if ((label.front() == U'-') || (label.back() == U'-')) {
      return std::unexpected(domain_errc::bad_input);
    }
  }

  /// Criterion 6
  if (transitional_processing) {
    constexpr auto is_valid = [](auto cp) {
      auto status = idna::code_point_status(cp);
      return (cp <= U'\x7e') || (status == idna::idna_status::valid);
    };

    if (std::cend(label) != std::find_if_not(std::cbegin(label), std::cend(label), is_valid)) {
      return std::unexpected(domain_errc::bad_input);
    }
  } else {
    constexpr auto is_valid_or_deviation = [](auto cp) {
      auto status = idna::code_point_status(cp);
      return (cp <= U'\x7e') || (status == idna::idna_status::valid) || (status == idna::idna_status::deviation);
    };

    if (std::cend(label) != std::find_if_not(std::cbegin(label), std::cend(label), is_valid_or_deviation)) {
      return std::unexpected(domain_errc::bad_input);
    }
  }

  return {};
}

///
struct domain_to_ascii_context {
  /// Stores the domain as UTF-32
  std::u32string domain_name;

  /// Parameters
  std::string* ascii_domain;
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
inline auto create_domain_to_ascii_context(std::string_view domain_name, std::string* ascii_domain, bool check_hyphens,
                                           bool check_bidi, bool check_joiners, bool use_std3_ascii_rules,
                                           bool transitional_processing, bool verify_dns_length)
    -> std::expected<domain_to_ascii_context, domain_errc> {
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
    return std::unexpected(domain_errc::encoding_error);
  }
}

///
/// \param context
/// \return
inline auto domain_to_ascii_impl(domain_to_ascii_context&& context) -> std::expected<void, domain_errc> {
  /// https://www.unicode.org/reports/tr46/#ToASCII

  constexpr auto map_domain_name =
      [](domain_to_ascii_context&& ctx) -> std::expected<domain_to_ascii_context, domain_errc> {
    auto result = idna::map_code_points(ctx.domain_name, ctx.use_std3_ascii_rules, ctx.transitional_processing);
    if (result) {
      ctx.domain_name.erase(result.value(), std::cend(ctx.domain_name));
      return std::move(ctx);
    } else {
      return std::unexpected(result.error());
    }
  };

  constexpr auto process_labels = [](auto&& ctx) -> std::expected<std::decay_t<decltype(ctx)>, domain_errc> {
    using namespace std::string_view_literals;

    constexpr auto to_string_view = [](auto&& label) {
      auto size = std::ranges::distance(label);
      return std::u32string_view(std::addressof(*std::cbegin(label)), size);
    };

    for (auto&& label :
         ctx.domain_name | std::ranges::views::split(U'.') | std::ranges::views::transform(to_string_view)) {
      if ((label.size() >= 4) && (label.substr(0, 4) == U"xn--")) {
        ctx.punycode_decoded.clear();
        auto decoded = punycode_decode(label.substr(4), &ctx.punycode_decoded);
        if (!decoded) {
          return std::unexpected(decoded.error());
        }

        auto validated = validate_label(ctx.punycode_decoded, ctx.use_std3_ascii_rules, ctx.check_hyphens,
                                        ctx.check_bidi, ctx.check_joiners, false);
        if (!validated) {
          return std::unexpected(validated.error());
        }
      } else {
        auto validated = validate_label(label, ctx.use_std3_ascii_rules, ctx.check_hyphens, ctx.check_bidi,
                                        ctx.check_joiners, ctx.transitional_processing);
        if (!validated) {
          return std::unexpected(validated.error());
        }
      }

      constexpr auto is_ascii = [](std::u32string_view input) noexcept {
        constexpr auto is_in_ascii_set = [](auto c) { return c <= U'\x7e'; };

        return std::ranges::cend(input) == std::ranges::find_if_not(input, is_in_ascii_set);
      };

      ctx.labels.emplace_back();
      if (!is_ascii(label)) {
        ctx.punycode_encoded.clear();
        auto result = punycode_encode(label, &ctx.punycode_encoded);
        if (!result) {
          return std::unexpected(result.error());
        }
        std::ranges::copy(U"xn--"sv, std::back_inserter(ctx.labels.back()));
        std::ranges::copy(ctx.punycode_encoded, std::back_inserter(ctx.labels.back()));
      } else {
        std::ranges::copy(label, std::back_inserter(ctx.labels.back()));
      }
    }

    if (ctx.domain_name.back() == U'.') {
      ctx.labels.emplace_back();
    }

    return std::move(ctx);
  };

  constexpr auto check_length = [](auto&& ctx) -> std::expected<std::decay_t<decltype(ctx)>, domain_errc> {
    constexpr auto max_domain_length = 253;
    constexpr auto max_label_length = 63;

    if (ctx.verify_dns_length) {
      auto length = ctx.domain_name.size();
      if ((length < 1) || (length > max_domain_length)) {
        return std::unexpected(domain_errc::invalid_length);
      }

      for (const auto& label : ctx.labels) {
        auto label_length = label.size();
        if ((label_length < 1) || (label_length > max_label_length)) {
          return std::unexpected(domain_errc::invalid_length);
        }
      }
    }

    return std::move(ctx);
  };

  constexpr auto copy_to_output = [](domain_to_ascii_context&& ctx) -> std::expected<void, domain_errc> {
    for (auto it = ctx.labels.begin(); it != ctx.labels.end(); ++it) {
      if (it != ctx.labels.begin()) {
        ctx.ascii_domain->push_back('.');
      }
      std::ranges::copy(*it, std::back_inserter(*ctx.ascii_domain));
    }
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
inline auto domain_to_ascii(std::string_view domain_name, std::string* ascii_domain, bool check_hyphens,
                            bool check_bidi, bool check_joiners, bool use_std3_ascii_rules,
                            bool transitional_processing, bool verify_dns_length) -> std::expected<void, domain_errc> {
  return create_domain_to_ascii_context(domain_name, ascii_domain, check_hyphens, check_bidi, check_joiners,
                                        use_std3_ascii_rules, transitional_processing, verify_dns_length)
      .and_then(domain_to_ascii_impl);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \param ascii_domain Output pointer to store the ASCII domain
/// \param be_strict Tells the processor to be strict
/// \param validation_error Optional pointer to a bool that will be set if a validation error occurs
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain_name, std::string* ascii_domain, bool be_strict,
                            bool* validation_error) -> std::expected<void, domain_errc> {
  auto result = domain_to_ascii(domain_name, ascii_domain, false, true, true, be_strict, false, be_strict);
  if (!result) {
    *validation_error |= true;
  } else if (ascii_domain->empty()) {
    *validation_error |= true;
    return std::unexpected(domain_errc::empty_string);
  }
  return result;
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \param ascii_domain Output pointer to store the ASCII domain
/// \param be_strict Tells the processor to be strict
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain_name, std::string* ascii_domain, bool be_strict)
    -> std::expected<void, domain_errc> {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_ascii(domain_name, ascii_domain, be_strict, &validation_error);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \param ascii_domain Output pointer to store the ASCII domain
/// \param validation_error Optional pointer to a bool that will be set if a validation error occurs
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain_name, std::string* ascii_domain, bool* validation_error) {
  return domain_to_ascii(domain_name, ascii_domain, false, validation_error);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain_name A domain
/// \param ascii_domain Output pointer to store the ASCII domain
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain_name, std::string* ascii_domain) {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_ascii(domain_name, ascii_domain, false, &validation_error);
}

struct domain_to_u8_context {
  std::string_view domain_name;

  /// Parameters
  std::string* u8_domain;

  std::vector<std::string> labels;

  /// This is used as an intermediate buffer
  std::u32string punycode_decoded;
};

///
/// \param context
/// \return
inline auto domain_to_u8_impl(domain_to_u8_context&& context) -> std::expected<void, domain_errc> {
  static constexpr auto to_string_view = [](auto&& label) {
    return std::string_view(std::addressof(*std::begin(label)), std::ranges::distance(label));
  };

  for (auto&& label :
       context.domain_name | std::ranges::views::split('.') | std::ranges::views::transform(to_string_view)) {
    context.labels.emplace_back();
    if (label.substr(0, 4) == "xn--") {
      label.remove_prefix(4);
      context.punycode_decoded.clear();
      auto result = punycode_decode(label, &context.punycode_decoded);
      if (!result) {
        return std::unexpected(result.error());
      }
      auto u8 = context.punycode_decoded | unicode::transforms::to_u8;
      auto first = std::cbegin(u8);
      auto last = std::cend(u8);
      for (auto it = first; it != last; ++it) {
        context.labels.back().push_back((*it).value());
      }
    } else {
      std::ranges::copy(label, std::back_inserter(context.labels.back()));
    }
  }

  if (context.domain_name.back() == U'.') {
    context.labels.emplace_back();
  }

  for (auto it = context.labels.begin(); it != context.labels.end(); ++it) {
    if (it != context.labels.begin()) {
      context.u8_domain->push_back('.');
    }
    std::ranges::copy(*it, std::back_inserter(*context.u8_domain));
  }

  return {};
}

/// Converts a Punycode encoded domain to UTF-8
///
/// \param domain_name A Punycode encoded domain
/// \param u8_domain Output pointer to store the UTF-8 domain
/// \param validation_error Optional pointer to a bool that will be set if a validation error occurs
/// \returns A valid UTF-8 encoded domain, or an error
inline auto domain_to_u8(std::string_view domain_name, std::string* u8_domain, [[maybe_unused]] bool* validation_error)
    -> std::expected<void, domain_errc> {
  auto context = domain_to_u8_context{domain_name, u8_domain, {}, {}};
  return domain_to_u8_impl(std::move(context));
}

/// Converts a Punycode encoded domain to UTF-8
///
/// \param domain_name A Punycode encoded domain
/// \param u8_domain Output pointer to store the UTF-8 domain
/// \returns A valid UTF-8 encoded domain, or an error
inline auto domain_to_u8(std::string_view domain_name, std::string* u8_domain) -> std::expected<void, domain_errc> {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_u8(domain_name, u8_domain, &validation_error);
}
}  // namespace skyr

#endif  // SKYR_DOMAIN_DOMAIN_HPP
