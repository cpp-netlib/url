// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_DOMAIN_DOMAIN_HPP
#define SKYR_DOMAIN_DOMAIN_HPP

#include <string>
#include <string_view>
#include <system_error>
#include <tl/expected.hpp>
#include <skyr/domain/errors.hpp>

namespace skyr {
inline namespace v1 {
/// Converts a UTF-32 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain A domain
/// \param be_strict Tells the processor to be strict
/// \returns An ASCII domain, or an error
auto domain_to_ascii(
    std::u32string_view domain,
    bool be_strict=false) -> tl::expected<std::string, std::error_code>;

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain A domain
/// \param be_strict Tells the processor to be strict
/// \returns An ASCII domain, or an error
auto domain_to_ascii(
    std::string_view domain,
    bool be_strict=false) -> tl::expected<std::string, std::error_code>;

/// Converts a Punycode encoded domain to UTF-8
///
/// \param ascii A Punycode encoded domain
/// \returns A valid UTF-8 encoded domain, or an error
auto domain_to_unicode(std::string_view ascii) -> tl::expected<std::string, std::error_code>;
}  // namespace v1
}  // namespace skyr

#endif // SKYR_DOMAIN_DOMAIN_HPP
