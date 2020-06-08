// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_DOMAIN_DOMAIN_HPP
#define SKYR_V1_DOMAIN_DOMAIN_HPP

#include <string>
#include <string_view>
#include <tl/expected.hpp>
#include <skyr/v1/domain/errors.hpp>

namespace skyr {
inline namespace v1 {
/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain A domain
/// \param be_strict Tells the processor to be strict
/// \param validation_error
/// \returns An ASCII domain, or an error
auto domain_to_ascii(
    std::string_view domain,
    bool be_strict,
    bool *validation_error) -> tl::expected<std::string, domain_errc>;

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain A domain
/// \param be_strict Tells the processor to be strict
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(
    std::string_view domain,
    bool be_strict) -> tl::expected<std::string, domain_errc> {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_ascii(domain, be_strict, &validation_error);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain A domain
/// \param validation_error
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain, bool *validation_error) {
  return domain_to_ascii(domain, false, validation_error);
}

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.domain.org/reports/tr46/#Processing)
///
/// \param domain A domain
/// \returns An ASCII domain, or an error
inline auto domain_to_ascii(std::string_view domain) {
  [[maybe_unused]] bool validation_error = false;
  return domain_to_ascii(domain, false, &validation_error);
}

/// Converts a Punycode encoded domain to UTF-8
///
/// \param ascii A Punycode encoded domain
/// \returns A valid UTF-8 encoded domain, or an error
auto domain_to_u8(std::string_view ascii) -> tl::expected<std::string, domain_errc>;
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_DOMAIN_DOMAIN_HPP
