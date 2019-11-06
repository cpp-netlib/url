// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_DOMAIN_HPP
#define SKYR_UNICODE_DOMAIN_HPP

/// \file domain.hpp

#include <string>
#include <string_view>
#include <system_error>
#include <tl/expected.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
/// \enum domain_errc
/// Enumerates domain processing errors
enum class domain_errc {
  /// The domain code point is disallowed
  disallowed_code_point = 1,
  /// The encoder or decoder received bad input
  bad_input,
  /// Overflow
  overflow,
  /// Unicode encoding error
  encoding_error,
};

/// Creates a `std::error_code` given a `skyr::domain_errc` value
/// \param error A domain error
/// \returns A `std::error_code` object
std::error_code make_error_code(domain_errc error) noexcept;

/// Performs Punycode encoding based on a reference implementation
/// defined in [RFC 3492](https://tools.ietf.org/html/rfc3492)
///
/// \param input A UTF-8 encoded domain to be encoded
/// \returns The encoded ASCII domain, or an error
tl::expected<std::string, std::error_code> punycode_encode(
    std::string_view input);

/// Performs Punycode encoding based on a reference implementation
/// defined in [RFC 3492](https://tools.ietf.org/html/rfc3492)
///
/// \param input A UTF-32 encoded domain to be encoded
/// \returns The encoded ASCII domain, or an error
tl::expected<std::string, std::error_code> punycode_encode(
    std::u32string_view input);

/// Performs Punycode decoding based on a reference implementation
/// defined in [RFC 3492](https://tools.ietf.org/html/rfc3492)
///
/// \param input An ASCII encoded domain to be decoded
/// \returns The decoded UTF-8 domain, or an error
tl::expected<std::string, std::error_code> punycode_decode(
    std::string_view input);

/// Converts a UTF-8 encoded domain to ASCII using
/// [IDNA processing](https://www.unicode.org/reports/tr46/#Processing)
///
/// \param domain A domain
/// \param be_strict Tells the processor to be strict
/// \returns An ASCII domain, or an error
tl::expected<std::string, std::error_code> domain_to_ascii(
    std::string_view domain,
    bool be_strict = true);

/// Converts a UTF-32 encoded domain to ASCII using
/// [IDNA processing](https://www.unicode.org/reports/tr46/#Processing)
///
/// \param domain A domain
/// \param be_strict Tells the processor to be strict
/// \returns An ASCII domain, or an error
tl::expected<std::string, std::error_code> domain_to_ascii(
    std::u32string_view domain,
    bool be_strict = true);
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::unicode::domain_errc> : true_type {};
}  // namespace std

#endif // SKYR_UNICODE_DOMAIN_HPP
