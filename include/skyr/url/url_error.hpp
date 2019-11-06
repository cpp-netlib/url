// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_ERROR_INC
#define SKYR_URL_ERROR_INC

/// \file skyr/url/url_error.hpp

#include <system_error>

using namespace std::literals;

namespace skyr {
inline namespace v1 {
/// \enum url_parse_errc
/// Enumerates URL parser errors
enum class url_parse_errc {
  /// The string contains an invalid Unicode character
  invalid_unicode_character = 1,
  /// A character is not a valid scheme character
  invalid_scheme_character,
  /// The URL is not an absolute URL with fragment
  not_an_absolute_url_with_fragment,
  /// Cannot set scheme value
  cannot_override_scheme,
  /// The ostname is empty
  empty_hostname,
  /// Invalid IPv4 address
  invalid_ipv4_address,
  /// Invalid IPv6 address
  invalid_ipv6_address,
  /// A character is a forbidden host point
  forbidden_host_point,
  /// Unable to decode host point
  cannot_decode_host_point,
  /// Invalid domain string
  domain_error,
  /// The URL cannot be a base URL
  cannot_be_a_base_url,
  /// The URL cannot have a username, password or port
  cannot_have_a_username_password_or_port,
  /// Invalid port value
  invalid_port,
};

/// Creates a `std::error_code` given a `skyr::url_parse_errc` value
/// \param error A URL parse error
/// \returns A `std::error_code` object
std::error_code make_error_code(url_parse_errc error) noexcept;
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::url_parse_errc> : true_type {};
}  // namespace std

#endif // SKYR_URL_ERROR_INC
