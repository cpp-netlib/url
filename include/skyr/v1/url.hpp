// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_URL_HPP
#define SKYR_V1_URL_HPP

#include <ostream>
#include <skyr/config.hpp>
#include <skyr/v1/core/errors.hpp>
#include <skyr/v1/core/url_record.hpp>
#include <skyr/v1/network/ipv4_address.hpp>
#include <skyr/v1/network/ipv6_address.hpp>
#include <skyr/v1/unicode/details/to_u8.hpp>
#include <skyr/v1/url_search_parameters.hpp>
#include <string>
#include <string_view>
#include <tl/expected.hpp>
#include <type_traits>

#if defined(SKYR_PLATFORM_MSVC)
#pragma warning(push)
#pragma warning(disable : 4251 4231 4660)
#endif // defined(SKYR_PLATFORM_MSVC)

/// \namespace skyr
/// Top-level namespace for URL parsing, unicode encoding and domain
/// name parsing.
namespace skyr {
inline namespace v1 {
/// Thrown when there is an error parsing the URL
class url_parse_error : public std::runtime_error {
 public:
  /// Constructor
  /// \param code An error code value
  explicit url_parse_error(std::error_code code) noexcept
      : runtime_error("URL parse error"), code_(code) {}

  /// \returns An error code
  [[nodiscard]] auto code() const noexcept {
    return code_;
  }

 private:

  std::error_code code_;

};

/// This class represents a URL. Parsing on construction is
/// performed according to the
/// [WhatWG specification](https://url.spec.whatwg.org/)
///
/// The API follows closely the
/// [WhatWG IDL specification](https://url.spec.whatwg.org/#url-class).
class url {

 public:

  /// The internal ASCII string type, or `std::basic_string<value_type>`
  using string_type = std::string;
  /// The internal string view type, or `std::basic_string_view<value_type>`
  using string_view = std::string_view;
  /// The ASCII character type
  using value_type = string_view::value_type;
  /// A constant iterator with a value type of `value_type`
  using const_iterator = string_view::const_iterator;
  /// An alias to `const_iterator`
  using iterator = const_iterator;
  /// A constant reference with value type of `value_type`
  using const_reference = string_view::const_reference;
  /// An alias to `const_reference`
  using reference = const_reference;
  /// \c std::size_t
  using size_type = std::size_t;

  /// Constructs an empty `url` object
  ///
  /// \post `empty() == true`
  url() : url_(), href_(), view_(href_), parameters_(this) {}

  /// Parses a URL from the input string. The input string can be
  /// any unicode encoded string (UTF-8, UTF-16 or UTF-32).
  ///
  /// \tparam Source The input string type
  /// \param input The input string
  /// \throws url_parse_error on parse errors
  template<class Source>
  explicit url(const Source &input)
      : url() {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(input);
    if (!bytes) {
      SKYR_EXCEPTIONS_THROW(url_parse_error(
          make_error_code(url_parse_errc::invalid_unicode_character)));
    }
    initialize(bytes.value());
  }

  /// Parses a URL from the input string. The input string can be
  /// any unicode encoded string (UTF-8, UTF-16 or UTF-32).
  ///
  /// \tparam Source The input string type
  /// \param input The input string
  /// \param base A base URL
  /// \throws url_parse_error on parse errors
  template<class Source>
  url(const Source &input, const url &base)
      : url() {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(input);
    if (!bytes) {
      SKYR_EXCEPTIONS_THROW(url_parse_error(
          make_error_code(url_parse_errc::invalid_unicode_character)));
    }
    const auto &base_record = base.record();
    initialize(bytes.value(), &base_record);
  }

  /// Constructs a URL from an existing record
  ///
  /// \param input A URL record
  explicit url(url_record &&input)
      : url() {
    update_record(std::forward<url_record>(input));
  }

  /// Copy constructor
  /// \param other Another `url` object
  url(const url &other)
      : url(url_record(other.url_)) {}

  /// Move constructor
  /// \param other Another `url` object
  url(url &&other) noexcept
      : url(std::move(other.url_)) {}

  /// Copy assignment operator
  /// \param other Another `url` object
  /// \return *this
  auto operator=(const url &other) -> url & = default;

  /// Move assignment operator
  /// \param other Another `url` object
  /// \return *this
  auto operator=(url&& other) -> url & = default;

  /// Destructor
  ~url() = default;

  /// Swaps this `url` object with another
  ///
  /// \param other Another `url` object
  void swap(url &other) noexcept {
    using std::swap;
    swap(url_, other.url_);
    swap(href_, other.href_);
    view_ = string_view(href_);
    other.view_ = string_view(other.href_);
    swap(parameters_, other.parameters_);
  }

  /// Returns the [serialization of the context object’s url](https://url.spec.whatwg.org/#dom-url-href)
  ///
  /// Equivalent to `skyr::serialize(url_).value()`
  ///
  /// \returns The serialization of the context object's url
  /// \sa to_json
  [[nodiscard]] auto href() const -> string_type {
    return href_;
  }

  /// Sets the context object's url according to the
  /// [steps described in the specification](https://url.spec.whatwg.org/#dom-url-href)

  /// \tparam Source The input string type
  /// \param href The input string
  /// \returns An error on failure to parse the new URL
  template<class Source>
  auto set_href(const Source &href) -> std::error_code {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(href);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }
    return set_href(std::string_view(bytes.value()));
  }

  /// Sets the context object's url according to the
  /// [steps described in the specification](https://url.spec.whatwg.org/#dom-url-href)

  /// \tparam Source The input string type
  /// \param href The input string
  /// \returns An error on failure to parse the new URL
  auto set_href(string_view href) -> std::error_code;

  /// Returns the [serialization of the context object’s url](https://url.spec.whatwg.org/#dom-url-href)
  ///
  /// Equivalent to `skyr::serialize(url_).value()`
  ///
  /// \returns The serialization of the context object's url
  /// \sa href()
  [[nodiscard]] auto to_json() const -> string_type {
    return href_;
  }

  /// Returns the [URL origin](https://url.spec.whatwg.org/#origin)
  ///
  /// \returns The [URL origin](https://url.spec.whatwg.org/#origin)
  [[nodiscard]] auto origin() const -> string_type;

  /// The URL scheme
  [[nodiscard]] auto scheme() const -> string_type {
    return url_.scheme;
  }

  /// The URL scheme + `":"`
  ///
  /// \returns The [URL protocol](https://url.spec.whatwg.org/#dom-url-protocol)
  [[nodiscard]] auto protocol() const -> string_type {
    return url_.scheme + ":";
  }

  /// Sets the [URL protocol](https://url.spec.whatwg.org/#dom-url-protocol)
  ///
  /// \tparam Source The input string type
  /// \param protocol The new URL protocol
  /// \returns An error on failure to parse the new URL
  template<class Source>
  auto set_protocol(const Source &protocol) -> std::error_code {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(protocol);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }

    return set_protocol(std::string_view(bytes.value()));
  }

  /// Sets the [URL protocol](https://url.spec.whatwg.org/#dom-url-protocol)
  ///
  /// \param protocol The new URL protocol
  /// \returns An error on failure to parse the new URL
  auto set_protocol(string_view protocol) -> std::error_code;

  /// \returns The [URL username](https://url.spec.whatwg.org/#dom-url-username)
  [[nodiscard]] auto username() const -> string_type {
    return url_.username;
  }

  /// Sets the [URL username](https://url.spec.whatwg.org/#dom-url-username)
  ///
  /// \tparam Source The input string type
  /// \param username The new username
  /// \returns An error on failure to parse the new URL
  template<class Source>
  auto set_username(const Source &username) -> std::error_code {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(username);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }
    return set_username(std::string_view(bytes.value()));
  }

  /// Sets the [URL username](https://url.spec.whatwg.org/#dom-url-username)
  ///
  /// \param username The new username
  /// \returns An error on failure to parse the new URL
  auto set_username(string_view username) -> std::error_code;

  /// The [URL password](https://url.spec.whatwg.org/#dom-url-password)
  ///
  /// Equivalent to: `url_.password? url_.password.value() : string_type()`
  ///
  /// \returns The URL password
  [[nodiscard]] auto password() const -> string_type {
    return url_.password;
  }

  /// Sets the [URL password](https://url.spec.whatwg.org/#dom-url-password)
  ///
  /// \tparam Source The input string type
  /// \param password The new password
  /// \returns An error on failure to parse the new URL
  template<class Source>
  auto set_password(const Source &password) -> std::error_code {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(password);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }
    return set_password(std::string_view(bytes.value()));
  }

  /// Sets the [URL password](https://url.spec.whatwg.org/#dom-url-password)
  ///
  /// \param password The new password
  /// \returns An error on failure to parse the new URL
  auto set_password(string_view password) -> std::error_code;

  /// \returns The [URL host](https://url.spec.whatwg.org/#dom-url-host)
  [[nodiscard]] auto host() const -> string_type {
    if (!url_.host) {
      return {};
    }

    if (!url_.port) {
      return url_.host.value().serialize();
    }

    return url_.host.value().serialize() + ":" + std::to_string(url_.port.value());
  }

  /// Sets the [URL host](https://url.spec.whatwg.org/#dom-url-host)
  ///
  /// \tparam Source The input string type
  /// \param host The new URL host
  /// \returns An error on failure to parse the new URL
  template<class Source>
  auto set_host(const Source &host) -> std::error_code {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(host);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }
    return set_host(std::string_view(bytes.value()));
  }

  /// Sets the [URL host](https://url.spec.whatwg.org/#dom-url-host)
  ///
  /// \param host The new URL host
  /// \returns An error on failure to parse the new URL
  auto set_host(string_view host) -> std::error_code;

  /// \returns The [URL hostname](https://url.spec.whatwg.org/#dom-url-hostname)
  [[nodiscard]] auto hostname() const -> string_type {
    if (!url_.host) {
      return {};
    }

    return url_.host.value().serialize();
  }

  /// Sets the [URL hostname](https://url.spec.whatwg.org/#dom-url-hostname)
  ///
  /// \tparam Source The input string type
  /// \param hostname The new URL host name
  /// \returns An error on failure to parse the new URL
  template<class Source>
  auto set_hostname(const Source &hostname) -> std::error_code {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(hostname);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }
    return set_hostname(std::string_view(bytes.value()));
  }

  /// Sets the [URL hostname](https://url.spec.whatwg.org/#dom-url-hostname)
  ///
  /// \param hostname The new URL host name
  /// \returns An error on failure to parse the new URL
  auto set_hostname(string_view hostname) -> std::error_code;

  /// Checks if the hostname is a valid domain name
  [[nodiscard]] auto is_domain() const -> bool {
    return url_.is_special() && !hostname().empty() && !is_ipv4_address() && !is_ipv6_address();
  }

  /// Returns an optional domain name
  [[nodiscard]] auto domain() const -> std::optional<string_type>;

  /// Returns an optional domain after decoding as a UTF-8 string
  /// \returns
  [[nodiscard]] auto u8domain() const -> std::optional<std::string>;

  /// Checks if the hostname is a valid IPv4 address
  [[nodiscard]] auto is_ipv4_address() const -> bool;

  /// Returns an optional ipv4_address value if the hostname is a
  /// valid IPv4 address
  [[nodiscard]] auto ipv4_address() const -> std::optional<skyr::ipv4_address>;

  /// Checks if the hostname is a valid IPv6 address
  [[nodiscard]] auto is_ipv6_address() const -> bool;

  /// Returns an optional ipv6_address value if the hostname is a
  /// valid IPv6 address
  [[nodiscard]] auto ipv6_address() const -> std::optional<skyr::ipv6_address>;

  /// Checks if the hostname is a valid opaque host
  [[nodiscard]] auto is_opaque_host() const -> bool {
    return url_.host && url_.host.value().is_opaque_host();
  }

  /// Checks if the hostname is a valid domain name
  [[nodiscard]] auto is_empty_host() const -> bool {
    return url_.host && url_.host.value().is_empty();
  }

  /// Returns the [URL port](https://url.spec.whatwg.org/#dom-url-port)
  ///
  /// \returns The [URL port](https://url.spec.whatwg.org/#dom-url-port)
  [[nodiscard]] auto port() const -> string_type {
    if (!url_.port) {
      return {};
    }

    return std::to_string(url_.port.value());
  }

  /// Returns the [URL port](https://url.spec.whatwg.org/#dom-url-port)
  ///
  /// \returns The [URL port](https://url.spec.whatwg.org/#dom-url-port)
  template<typename intT>
  [[nodiscard]] auto port(
      std::enable_if_t<std::is_integral_v<intT>> * = nullptr) const -> std::optional<intT> {
    auto p = port();
    if (p.empty()) {
      return std::nullopt;
    }

    const char *port_first = p.data();
    char *port_last = nullptr;
    return static_cast<intT>(
        std::strtoul(port_first, &port_last, 10));
  }

  /// Sets the [URL port](https://url.spec.whatwg.org/#dom-url-port)
  ///
  /// \tparam PortSource The input type
  /// \param port The new port
  /// \returns An error on failure to parse the new URL
  template<class PortSource>
  auto set_port(const PortSource &port) -> std::error_code {
    return set_port_impl(port);
  }

  /// Sets the [URL port](https://url.spec.whatwg.org/#dom-url-port)
  ///
  /// \param port The new port
  /// \returns An error on failure to parse the new URL
  auto set_port(string_view port) -> std::error_code;

  /// Returns the [URL pathname](https://url.spec.whatwg.org/#dom-url-pathname)
  ///
  /// \returns The URL pathname
  [[nodiscard]] auto pathname() const -> string_type {
    if (url_.cannot_be_a_base_url) {
      return url_.path.front();
    }

    if (url_.path.empty()) {
      return {};
    }

    auto pathname = string_type("/");
    for (const auto &segment : url_.path) {
      pathname += segment;
      pathname += "/";
    }
    return pathname.substr(0, pathname.length() - 1);
  }

  /// Sets the [URL pathname](https://url.spec.whatwg.org/#dom-url-pathname)
  ///
  /// \tparam Source The input string type
  /// \param pathname The new pathname
  /// \returns An error on failure to parse the new URL
  template<class Source>
  auto set_pathname(const Source &pathname) -> std::error_code {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(pathname);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }
    return set_pathname(std::string_view(bytes.value()));
  }

  /// Sets the [URL pathname](https://url.spec.whatwg.org/#dom-url-pathname)
  ///
  /// \param pathname The new pathname
  /// \returns An error on failure to parse the new URL
  auto set_pathname(string_view pathname) -> std::error_code;

  /// Returns the [URL search string](https://url.spec.whatwg.org/#dom-url-search)
  ///
  /// \returns The [URL search string](https://url.spec.whatwg.org/#dom-url-search)
  [[nodiscard]] auto search() const -> string_type {
    if (!url_.query || url_.query.value().empty()) {
      return {};
    }

    return "?" + url_.query.value();
  }

  /// Sets the [URL search string](https://url.spec.whatwg.org/#dom-url-search)
  ///
  /// \tparam Source The input string type
  /// \param search The new search string
  /// \returns An error on failure to parse the new URL
  template<class Source>
  auto set_search(const Source &search) -> std::error_code {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(search);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }
    return set_search(std::string_view(bytes.value()));
  }

  /// Sets the [URL search string](https://url.spec.whatwg.org/#dom-url-search)
  ///
  /// \param search The new search string
  /// \returns An error on failure to parse the new URL
  auto set_search(string_view search) -> std::error_code;

  /// \returns A reference to the search parameters
  [[nodiscard]] auto search_parameters() -> url_search_parameters & {
    return parameters_;
  }

  /// \returns A reference to the search parameters
  [[nodiscard]] auto search_parameters() const -> const url_search_parameters & {
    return parameters_;
  }

  /// Returns the [URL hash string](https://url.spec.whatwg.org/#dom-url-hash)
  ///
  /// \returns The [URL hash string](https://url.spec.whatwg.org/#dom-url-hash)
  [[nodiscard]] auto hash() const -> string_type {
    if (!url_.fragment || url_.fragment.value().empty()) {
      return {};
    }

    return "#" + url_.fragment.value();
  }

  /// Sets the [URL hash string](https://url.spec.whatwg.org/#dom-url-hash)
  ///
  /// \tparam Source The input string type
  /// \param hash The new hash string
  /// \returns An error on failure to parse the new URL
  template<class Source>
  auto set_hash(const Source &hash) -> std::error_code {
    static_assert(
        is_url_convertible_v<Source>,
        "Source is not a valid URL string type");

    auto bytes = details::to_u8(hash);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }
    return set_hash(std::string_view(bytes.value()));
  }

  /// Sets the [URL hash string](https://url.spec.whatwg.org/#dom-url-hash)
  ///
  /// \param hash The new hash string
  /// \returns An error on failure to parse the new URL
  auto set_hash(string_view hash) -> std::error_code;

  /// The URL context object
  ///
  /// \returns The underlying `url_record` implementation.
  [[nodiscard]] auto record() const & noexcept -> const url_record & {
    return url_;
  }

  /// The URL context object
  ///
  /// \returns The underlying `url_record` implementation.
  [[nodiscard]] auto record() && noexcept -> url_record && {
    return std::move(url_);
  }

  /// Tests whether the URL uses a
  /// [special scheme](https://url.spec.whatwg.org/#special-scheme)
  ///
  /// \returns `true` if the URL scheme is special, `false`
  ///          otherwise
  [[nodiscard]] auto is_special() const noexcept {
    return url_.is_special();
  }

  /// An iterator to the beginning of the context object's string (`href_`)
  ///
  /// \returns An iterator to the beginning of the context object's string
  [[nodiscard]] auto begin() const noexcept {
    return view_.begin();
  }

  /// An iterator to the end of the context object's string (`href_`)
  ///
  /// \returns An iterator to the end of the URL string
  [[nodiscard]] auto end() const noexcept {
    return view_.end();
  }

  /// Tests whether the URL is an empty string
  ///
  /// \returns `true` if the URL is an empty string, `false`
  ///          otherwise
  [[nodiscard]] auto empty() const noexcept {
    return view_.empty();
  }

  /// Gets the size of the URL buffer
  /// \return The size of the URL buffer
  [[nodiscard]] auto size() const noexcept {
    return view_.size();
  }

  /// Compares this `url` object lexicographically with another
  ///
  /// \param other The other `url` object
  /// \returns `href_.compare(other.href_)`
  [[nodiscard]] auto compare(const url &other) const noexcept {
    return view_.compare(other.view_);
  }

  /// Returns the default port for
  /// [special schemes](https://url.spec.whatwg.org/#special-scheme)
  ///
  /// \param scheme
  /// \returns The default port if the scheme is special, `nullopt`
  ///          otherwise
  [[nodiscard]] static auto default_port(
      std::string_view scheme) noexcept -> std::optional<std::uint16_t>;

  /// Clears the underlying URL string
  ///
  /// \post `empty() == true`
  void clear() {
    update_record(url_record{});
  }

  /// Returns the underlying byte buffer
  ///
  /// \returns `href_.c_str()`
  [[nodiscard]] auto c_str() const noexcept {
    return href_.c_str();
  }

  /// Returns the underlying byte buffer
  ///
  /// \returns `href_.data()`
  [[nodiscard]] auto data() const noexcept {
    return href_.data();
  }

  /// Returns the underlying string
  ///
  /// \returns `href_`
  explicit operator string_type() const {
    return href_;
  }

 private:

  void initialize(
      string_view input,
      const url_record *base);

  void initialize(
      string_view input);

  void update_record(url_record &&url);

  template<class Source>
  auto set_port_impl(
      const Source &port,
      typename std::enable_if_t<is_url_convertible_v<Source>> * = nullptr) -> std::error_code {
    auto bytes = details::to_u8(port);
    if (!bytes) {
      return make_error_code(url_parse_errc::invalid_unicode_character);
    }
    return set_port(std::string_view(bytes.value()));
  }

  template<typename intT>
  auto set_port_impl(
      intT port,
      std::enable_if_t<std::is_integral_v<intT>> * = nullptr) -> std::error_code {
    return set_port(string_view(std::to_string(port)));
  }

  url_record url_;
  std::string href_;
  string_view view_;
  url_search_parameters parameters_;

};

/// Swaps two `url` objects
///
/// Equivalent to `lhs.swap(rhs)`
///
/// \param lhs The first `url` object
/// \param rhs The second `url` object
inline void swap(url &lhs, url &rhs) noexcept {
  lhs.swap(rhs);
}

namespace details {
auto make_url(
    std::string_view input, const url_record *base) -> tl::expected<url, url_parse_errc>;
}  // namespace details

/// Parses a URL string and constructs a `url` object on success,
/// wrapped in a `tl::expected`
///
/// \tparam Source The input string type
/// \param input The input string
/// \returns A `url` object on success, an error on failure
template<class Source>
inline auto make_url(
    const Source &input) -> tl::expected<url, url_parse_errc> {
  auto bytes = details::to_u8(input);
  if (!bytes) {
    return tl::make_unexpected(url_parse_errc::invalid_unicode_character);
  }
  return details::make_url(bytes.value(), nullptr);
}

/// Parses a URL string and constructs a `url` object on success,
/// wrapped in a `tl::expected`
///
/// \tparam Source The input string type
/// \param input The input string
/// \param base The base URL
/// \returns A `url` object on success, an error on failure
template<class Source>
inline auto make_url(
    const Source &input, const url &base) -> tl::expected<url, url_parse_errc> {
  auto bytes = details::to_u8(input);
  if (!bytes) {
    return tl::make_unexpected(url_parse_errc::invalid_unicode_character);
  }
  const auto &base_record = base.record();
  return details::make_url(bytes.value(), &base_record);
}

/// Tests two URLs for equality according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-equivalence)
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `true` if the `url` objects are equal, `false` otherwise
inline auto operator==(const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

/// Tests two URLs for inequality
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `!(lhs == rhs)`
inline auto operator!=(const url &lhs, const url &rhs) noexcept {
  return !(lhs == rhs);
}

/// Comparison operator
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `lhs.compare(rhs) < 0`
inline auto operator<(const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

/// Comparison operator
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `lhs.compare(rhs) > 0`
inline auto operator>(const url &lhs, const url &rhs) noexcept {
  return rhs < lhs;
}

/// Comparison operator
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `!(lhs > rhs)
inline auto operator<=(const url &lhs, const url &rhs) noexcept {
  return !(lhs > rhs);
}

/// Comparison operator
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns !(lhs < rhs)
inline auto operator>=(const url &lhs, const url &rhs) noexcept {
  return !(lhs < rhs);
}

///
/// \param os
/// \param url
/// \returns
inline auto &operator<<(std::ostream &os, const url &url) {
  return os << url.href();
}

namespace literals {
/// Literal operator for a URL string
/// \param str
/// \param length
/// \return A url
inline auto operator "" _url(const char *str, std::size_t length) {
  return url(std::string_view(str, length));
}

///
/// \param str
/// \param length
/// \return
inline auto operator "" _url(const wchar_t *str, std::size_t length) {
  return url(std::wstring_view(str, length));
}

///
/// \param str
/// \param length
/// \return
inline auto operator "" _url(const char16_t *str, std::size_t length) {
  return url(std::u16string_view(str, length));
}

///
/// \param str
/// \param length
/// \return
inline auto operator "" _url(const char32_t *str, std::size_t length) {
  return url(std::u32string_view(str, length));
}
}  // namespace literals
}  // namespace v1
}  // namespace skyr

#if defined(SKYR_PLATFORM_MSVC)
#pragma warning(pop)
#endif // defined(SKYR_PLATFORM_MSVC)

#endif  // SKYR_V1_URL_HPP
