// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_CORE_ERRORS_HPP
#define SKYR_V2_CORE_ERRORS_HPP

#include <system_error>

namespace skyr::inline v2 {
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

namespace details {
class url_parse_error_category : public std::error_category {
 public:
  [[nodiscard]] auto name() const noexcept -> const char* override {
    return "url parse";
  }

  [[nodiscard]] auto message(int error) const noexcept -> std::string override {
    switch (static_cast<url_parse_errc>(error)) {
      case url_parse_errc::invalid_scheme_character:
        return "Invalid URL scheme";
      case url_parse_errc::not_an_absolute_url_with_fragment:
        return "Not an absolute URL with fragment";
      case url_parse_errc::empty_hostname:
        return "Empty hostname";
      case url_parse_errc::invalid_ipv4_address:
        return "Invalid IPv4 address";
      case url_parse_errc::invalid_ipv6_address:
        return "Invalid IPv6 address";
      case url_parse_errc::forbidden_host_point:
        return "Forbidden host point";
      case url_parse_errc::cannot_decode_host_point:
        return "Cannot decode host point";
      case url_parse_errc::domain_error:
        return "Domain error";
      case url_parse_errc::cannot_be_a_base_url:
        return "Cannot be a base URL";
      case url_parse_errc::cannot_have_a_username_password_or_port:
        return "Cannot have a username, password or port";
      case url_parse_errc::invalid_port:
        return "Invalid port";
      default:
        return "(Unknown error)";
    }
  }
};
}  // namespace details

/// Creates a `std::error_code` given a `skyr::url_parse_errc` value
/// \param error A URL parse error
/// \returns A `std::error_code` object
inline auto make_error_code(url_parse_errc error) noexcept -> std::error_code {
  static const details::url_parse_error_category category{};
  return std::error_code(static_cast<int>(error), category);
}
}  // namespace skyr::inline v2

namespace std {
template <>
struct is_error_code_enum<skyr::v2::url_parse_errc> : true_type {};
}  // namespace std

#endif  // SKYR_V2_CORE_ERRORS_HPP
