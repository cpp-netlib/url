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
/// \param domain_name A domain
/// \param be_strict Tells the processor to be strict
/// \param validation_error
/// \returns An ASCII domain, or an error
auto domain_to_ascii(
    std::string_view domain_name,
    bool be_strict,
    bool *validation_error) -> tl::expected<std::string, domain_errc>;

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
auto domain_to_u8(std::string_view domain_name, bool *validation_error) -> tl::expected<std::string, domain_errc>;

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
