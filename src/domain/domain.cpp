// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <skyr/unicode/ranges/transforms/byte_transform.hpp>
#include <skyr/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/domain/errors.hpp>
#include <skyr/domain/domain.hpp>
#include <skyr/domain/idna.hpp>
#include <skyr/domain/punycode.hpp>
#include "string/ascii.hpp"
#include "string/join.hpp"
#include "string/split.hpp"

namespace skyr {
inline namespace v1 {
namespace {
auto process(
    std::u32string_view domain_name, bool use_std3_ascii_rules,
    bool check_hyphens, bool check_bidi, bool check_joiners,
    bool transitional_processing) -> tl::expected<std::u32string, std::error_code> {
  auto result = std::u32string();
  auto error = false;

  auto first = begin(domain_name), last = end(domain_name);
  auto it = first;

  while (it != last) {
    switch (domain::map_idna_status(*it)) {
      case domain::idna_status::disallowed:error = true;
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
      case domain::idna_status::ignored:break;
      case domain::idna_status::mapped:result += domain::map_idna_code_point(*it);
        break;
      case domain::idna_status::deviation:
        if (transitional_processing) {
          result += domain::map_idna_code_point(*it);
        } else {
          result += *it;
        }
        break;
      case domain::idna_status::valid:result += *it;
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

auto unicode_to_ascii(
    std::u32string_view domain_name, bool check_hyphens, bool check_bidi,
    bool check_joiners, bool use_std3_ascii_rules, bool transitional_processing,
    bool verify_dns_length) -> tl::expected<std::string, std::error_code> {
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
      .or_else([](auto) -> tl::expected<std::string, std::error_code> {
        return tl::make_unexpected(make_error_code(domain_errc::encoding_error));
      });
}
}  // namespace

auto domain_to_ascii(
    std::string_view domain, bool be_strict) -> tl::expected<std::string, std::error_code> {
  auto utf32 = unicode::as<std::u32string>(unicode::view::as_u8(domain) | unicode::transform::to_u32);
  if (!utf32) {
    return tl::make_unexpected(make_error_code(domain_errc::encoding_error));
  }
  return domain_to_ascii(utf32.value(), be_strict);
}

auto domain_to_ascii(
    std::u32string_view domain, bool be_strict) -> tl::expected<std::string, std::error_code> {
  auto result =
      unicode_to_ascii(domain, false, true, true, be_strict, false, be_strict);
  if (!result) {
    // validation error
    return tl::make_unexpected(result.error());
  }
  return result;
}
}  // namespace v1
}  // namespace skyr
