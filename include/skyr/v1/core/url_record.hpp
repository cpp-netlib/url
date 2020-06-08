// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_URL_RECORD_HPP
#define SKYR_V1_CORE_URL_RECORD_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <skyr/v1/core/host.hpp>
#include <skyr/v1/core/schemes.hpp>

namespace skyr {
inline namespace v1 {
/// Represents the parts of a URL identifier.
class url_record {

 public:

  /// string type
  /// \sa url::string_type
  using string_type = std::string;

  /// An ASCII string that identifies the type of URL
  string_type scheme;
  /// An ASCII string identifying a username
  string_type username;
  /// An ASCII string identifying a password
  string_type password;
  /// An optional URL host, either a domain, IPv4 or IPv6 address,
  /// an opaque host, or empty
  std::optional<skyr::v1::host> host;
  /// An optional network port
  std::optional<std::uint16_t> port;
  /// A list of zero or more ASCII strings, used to identify a
  /// location in a hierarchical form
  std::vector<string_type> path;
  /// An optional ASCII string
  std::optional<string_type> query;
  /// An optional ASCII string
  std::optional<string_type> fragment;

  /// A Boolean value indicating whether this URL can be used as a
  /// base URL
  bool cannot_be_a_base_url = false;

  /// Default constructor
  url_record() = default;

  /// Tests if the URL is a special scheme
  /// \returns `true` if the URL scheme is a special scheme, `false`
  ///          otherwise
  [[nodiscard]] auto is_special() const noexcept -> bool {
    return skyr::is_special(scheme);
  }

  /// Tests if the URL includes credentials
  /// \returns `true` if the URL username or password is not an
  ///          empty string, `false` otherwise
  [[nodiscard]] auto includes_credentials() const noexcept -> bool {
    return !username.empty() || !password.empty();
  }

  /// Tests if the URL cannot have a username, password or port
  /// \returns `true` if the URL cannot have a username, password
  ///          or port
  [[nodiscard]] auto cannot_have_a_username_password_or_port() const noexcept -> bool {
    return
        (!host || host.value().is_empty()) ||
            cannot_be_a_base_url ||
            (scheme == "file");
  }

  /// Swaps two `url_record` objects
  /// \param other Another `url_record` object
  void swap(url_record &other) noexcept {
    using std::swap;
    swap(scheme, other.scheme);
    swap(username, other.username);
    swap(password, other.password);
    swap(host, other.host);
    swap(port, other.port);
    swap(path, other.path);
    swap(query, other.query);
    swap(fragment, other.fragment);
    swap(cannot_be_a_base_url, other.cannot_be_a_base_url);
  }
};

/// Swaps two `url_record` objects
///
/// Equivalent to `lhs.swap(rhs)`
///
/// \param lhs A `url_record` object
/// \param rhs A `url_record` object
inline void swap(url_record &lhs, url_record &rhs) noexcept {
  lhs.swap(rhs);
}
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_CORE_URL_RECORD_HPP
