// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_RECORD_INC
#define SKYR_URL_RECORD_INC

#include <vector>
#include <string>
#include <cstdint>
#include <optional>

namespace skyr {
inline namespace v1 {
/// Represents the parts of a URL identifier.
///
/// For example:
/// ```
/// auto url = skyr::url_parse("http://example.org/");
/// assert(url);
/// assert("http" == url.scheme);
/// assert(url.host);
/// assert("example.org" == url.host.value());
/// assert(1 == url.path.sixe());
/// ```
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
  std::optional<string_type> host;
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

  /// A Boolean value indicating whether a non-fatal validation
  /// error occurred during parsing
  bool validation_error = false;

  /// Default constructor
  url_record() = default;

  /// Tests if the URL is a special scheme
  /// \returns `true` if the URL scheme is a special scheme, `false`
  ///          otherwise
  [[nodiscard]] bool is_special() const noexcept;

  /// Tests if the URL includes credentials
  /// \returns `true` if the URL username or password is not an
  ///          empty string, `false` otherwise
  [[nodiscard]] bool includes_credentials() const noexcept;

  /// Tests if the URL cannot have a username, password or port
  /// \returns `true` if the URL cannot have a username, password
  ///          or port
  [[nodiscard]] bool cannot_have_a_username_password_or_port() const noexcept;

  /// Swaps two `url_record` objects
  /// \param other Another `url_record` object
  void swap(url_record &other) noexcept;

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

#endif // SKYR_URL_RECORD_INC
