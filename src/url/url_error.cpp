// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include "skyr/url/url_error.hpp"

namespace skyr {
inline namespace v1 {
namespace {
class url_parse_error_category : public std::error_category {
 public:
  const char *name() const noexcept override;
  std::string message(int error) const noexcept override;
};

const char *url_parse_error_category::name() const noexcept {
  return "parse";
}

std::string url_parse_error_category::message(int error) const noexcept {
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

const url_parse_error_category category{};
}  // namespace

std::error_code make_error_code(url_parse_errc error) noexcept {
  return std::error_code(static_cast<int>(error), category);
}
}  // namespace v1
}  // namespace skyr
