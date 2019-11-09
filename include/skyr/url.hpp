// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_INC
#define SKYR_URL_INC

#include <string>
#include <string_view>
#include <type_traits>
#include <ostream>
#include <tl/expected.hpp>
#include <skyr/config.hpp>
#include <skyr/version.hpp>
#include <skyr/url/url_record.hpp>
#include <skyr/url/url_error.hpp>
#include <skyr/url/url_search_parameters.hpp>
#include <skyr/url/details/to_bytes.hpp>

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
  /// \param error An error code value
  explicit url_parse_error(std::error_code error) noexcept
      : runtime_error("URL parse error"), error_(error) {}

  /// \returns An error code
  [[nodiscard]] std::error_code error() const noexcept {
    return error_;
  }

 private:

  std::error_code error_;

};

/// Represents a URL. Parsing is performed according to the
/// [WhatWG specification](https://url.spec.whatwg.org/)
///
/// The API follows closely the
/// [WhatWG IDL specification](https://url.spec.whatwg.org/#url-class).
///
/// For example:
/// ```
/// auto url = skyr::url("http://example.org/");
/// assert("http:" == url.scheme());
/// assert("example.org" == url.hostname());
/// assert("/" == url.pathname());
/// ```
class url {

 public:

  /// The internal ASCII string type, or `std::basic_string<value_type>`
  using string_type = std::string;
  /// The internal string_view, or `std::basic_string_view<value_type>`
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
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(input);
    if (!bytes) {
      SKYR_EXCEPTIONS_THROW(url_parse_error(
          make_error_code(url_parse_errc::invalid_unicode_character)));
    }
    initialize(std::string_view(bytes.value()));
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
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(input);
    if (!bytes) {
      SKYR_EXCEPTIONS_THROW(url_parse_error(
          make_error_code(url_parse_errc::invalid_unicode_character)));
    }
    initialize(std::string_view(bytes.value()), base.record());
  }

  /// Constructs a URL from an existing record
  ///
  /// \param input A URL record
  explicit url(url_record &&input)
      : url() {
    update_record(std::forward<url_record>(input));
  }

  ///
  /// \param other
  url(const url &other)
      : url(url_record(other.url_)) {}

  ///
  /// \param other
  url(url &&other) noexcept
      : url(std::move(other.url_)) {}

  /// Swaps this `url` object with another
  ///
  /// \param other The other `url` object
  void swap(url &other) noexcept;

  /// The URL string
  ///
  /// Equivalent to `skyr::serialize(url_).value()`
  ///
  /// \returns The underlying URL string
  [[nodiscard]] string_type href() const;

  /// \tparam Source The input string type
  /// \param href The input string
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_href(const Source &href) {
    static_assert(
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(href);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }
    return set_href(std::string_view(bytes.value()));
  }

  /// \param href The input string
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_href(string_view href);

  /// The URL string
  ///
  /// Equivalent to `skyr::serialize(url_).value()`
  ///
  /// \returns The underlying URL string
  /// \sa href()
  [[nodiscard]] string_type to_json() const;

  /// \returns The [URL origin](https://url.spec.whatwg.org/#origin)
  [[nodiscard]] string_type origin() const;

  /// The URL scheme + `":"`
  ///
  /// \returns The [URL protocol](https://url.spec.whatwg.org/#dom-url-protocol)
  [[nodiscard]] string_type protocol() const;

  /// Sets the [URL protocol](https://url.spec.whatwg.org/#dom-url-protocol)
  ///
  /// \tparam Source The input string type
  /// \param protocol The new URL protocol
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_protocol(const Source &protocol) {
    static_assert(
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(protocol);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }

    return set_protocol(std::string_view(bytes.value()));
  }

  /// Sets the [URL protocol](https://url.spec.whatwg.org/#dom-url-protocol)
  ///
  /// \param protocol The new URL protocol
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_protocol(string_view protocol);

  /// \returns The [URL username](https://url.spec.whatwg.org/#dom-url-username)
  [[nodiscard]] string_type username() const;

  /// Sets the [URL username](https://url.spec.whatwg.org/#dom-url-username)
  ///
  /// \tparam Source The input string type
  /// \param username The new username
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_username(const Source &username) {
    static_assert(
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(username);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }
    return set_username(std::string_view(bytes.value()));
  }

  /// Sets the [URL username](https://url.spec.whatwg.org/#dom-url-username)
  ///
  /// \param username The new username
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_username(string_view username);

  /// The [URL password](https://url.spec.whatwg.org/#dom-url-password)
  ///
  /// Equivalent to: `url_.password? url_.password.value() : string_type()`
  ///
  /// \returns The URL password
  [[nodiscard]] string_type password() const;

  /// Sets the [URL password](https://url.spec.whatwg.org/#dom-url-password)
  ///
  /// \tparam Source The input string type
  /// \param password The new password
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_password(const Source &password) {
    static_assert(
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(password);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }
    return set_password(std::string_view(bytes.value()));
  }

  /// Sets the [URL password](https://url.spec.whatwg.org/#dom-url-password)
  ///
  /// \param password The new password
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_password(string_view password);

  /// \returns The [URL host](https://url.spec.whatwg.org/#dom-url-host)
  [[nodiscard]] string_type host() const;

  /// Sets the [URL host](https://url.spec.whatwg.org/#dom-url-host)
  ///
  /// \tparam Source The input string type
  /// \param host The new URL host
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_host(const Source &host) {
    static_assert(
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(host);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }
    return set_host(std::string_view(bytes.value()));
  }

  /// Sets the [URL host](https://url.spec.whatwg.org/#dom-url-host)
  ///
  /// \param host The new URL host
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_host(string_view host);

  /// \returns The [URL hostname](https://url.spec.whatwg.org/#dom-url-hostname)
  [[nodiscard]] string_type hostname() const;

  /// Sets the [URL hostname](https://url.spec.whatwg.org/#dom-url-hostname)
  ///
  /// \tparam Source The input string type
  /// \param hostname The new URL host name
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_hostname(const Source &hostname) {
    static_assert(
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(hostname);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }
    return set_hostname(std::string_view(bytes.value()));
  }

  /// Sets the [URL hostname](https://url.spec.whatwg.org/#dom-url-hostname)
  ///
  /// \param hostname The new URL host name
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_hostname(string_view hostname);

  /// \returns The [URL port](https://url.spec.whatwg.org/#dom-url-port)
  [[nodiscard]] string_type port() const;

  /// \returns The [URL port](https://url.spec.whatwg.org/#dom-url-port)
  template<typename intT>
  [[nodiscard]] intT port(
      typename std::enable_if<std::is_integral<intT>::value>::type * = nullptr) const {
    auto p = port();
    const char *port_first = p.data();
    char *port_last = nullptr;
    return static_cast<intT>(
        std::strtoul(port_first, &port_last, 10));
  }

  /// Sets the [URL port](https://url.spec.whatwg.org/#dom-url-port)
  ///
  /// \tparam Source The input string type
  /// \param port The new port
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_port(const Source &port) {
    return set_port_impl(port);
  }

  /// Sets the [URL port](https://url.spec.whatwg.org/#dom-url-port)
  ///
  /// \param port The new port
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_port(string_view port);

  /// Returns the [URL pathname](https://url.spec.whatwg.org/#dom-url-pathname)
  ///
  /// \returns The URL pathname
  [[nodiscard]] string_type pathname() const;

  /// Sets the [URL pathname](https://url.spec.whatwg.org/#dom-url-pathname)
  ///
  /// \tparam Source The input string type
  /// \param pathname The new pathname
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_pathname(const Source &pathname) {
    static_assert(
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(pathname);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }
    return set_pathname(std::string_view(bytes.value()));
  }

  /// Sets the [URL pathname](https://url.spec.whatwg.org/#dom-url-pathname)
  ///
  /// \param pathname The new pathname
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_pathname(string_view pathname);

  /// Returns the [URL search string](https://url.spec.whatwg.org/#dom-url-search)
  ///
  /// \returns The URL search string
  [[nodiscard]] string_type search() const;

  /// Sets the [URL search string](https://url.spec.whatwg.org/#dom-url-search)
  ///
  /// \tparam Source The input string type
  /// \param search The new search string
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_search(const Source &search) {
    static_assert(
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(search);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }
    return set_search(std::string_view(bytes.value()));
  }

  /// Sets the [URL search string](https://url.spec.whatwg.org/#dom-url-search)
  ///
  /// \param search The new search string
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_search(string_view search);

  /// \returns A reference to the search parameters
  [[nodiscard]] url_search_parameters &search_parameters();

  /// Returns the [URL hash string](https://url.spec.whatwg.org/#dom-url-hash)
  ///
  /// \returns The URL hash string
  [[nodiscard]] string_type hash() const;

  /// Sets the [URL hash string](https://url.spec.whatwg.org/#dom-url-hash)
  ///
  /// \tparam Source The input string type
  /// \param hash The new hash string
  /// \returns An error on failure to parse the new URL
  template<class Source>
  tl::expected<void, std::error_code> set_hash(const Source &hash) {
    static_assert(
        is_url_convertible<Source>::value,
        "Source is not a valid URL string type");

    auto bytes = details::to_bytes(hash);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }
    return set_hash(std::string_view(bytes.value()));
  }

  /// Sets the [URL hash string](https://url.spec.whatwg.org/#dom-url-hash)
  ///
  /// \param hash The new hash string
  /// \returns An error on failure to parse the new URL
  tl::expected<void, std::error_code> set_hash(string_view hash);

  /// \returns The underlying `url_record` implementation.
  [[nodiscard]] const url_record &record() const & noexcept {
    return url_;
  }

  /// \returns The underlying `url_record` implementation.
  [[nodiscard]] url_record &&record() && noexcept {
    return std::move(url_);
  }

  /// Tests whether the URL uses a
  /// [special scheme](https://url.spec.whatwg.org/#special-scheme)
  ///
  /// \returns `true` if the URL scheme is special, `false`
  ///          otherwise
  [[nodiscard]] bool is_special() const noexcept {
    return url_.is_special();
  }

  /// A [validation error](https://url.spec.whatwg.org/#validation-error)
  /// indicates a mismatch between input and valid input
  ///
  /// \returns `true` if there was a validation error during
  ///          parsing, `false` otherwise
  [[nodiscard]] bool validation_error() const noexcept {
    return url_.validation_error;
  }

  /// \returns An iterator to the beginning of the URL string
  [[nodiscard]] const_iterator begin() const noexcept {
    return view_.begin();
  }

  /// \returns An iterator to the end of the URL string
  [[nodiscard]] const_iterator end() const noexcept {
    return view_.end();
  }

  /// Tests whether the URL is an empty string
  ///
  /// \returns `true` if the URL is an empty string, `false`
  ///          otherwise
  [[nodiscard]] bool empty() const noexcept {
    return view_.empty();
  }

  /// Gets the size of the URL buffer
  /// \return The size of the URL buffer
  [[nodiscard]] size_type size() const noexcept {
    return view_.size();
  }

  /// Compares this `url` object lexicographically with another
  ///
  /// \param other The other `url` object
  /// \returns `href_.compare(other.href_)`
  [[nodiscard]] int compare(const url &other) const noexcept {
    return view_.compare(other.view_);
  }

  /// Returns the default port for
  /// [special schemes](https://url.spec.whatwg.org/#special-scheme)
  ///
  /// \param scheme
  /// \returns The default port if the scheme is special, `nullopt`
  ///          otherwise
  [[nodiscard]] static std::optional<std::uint16_t> default_port(
      std::string_view scheme) noexcept;

  /// Clears the underlying URL string
  ///
  /// \post `empty() == true`
  void clear();

  /// Returns the underyling byte buffer
  ///
  /// \returns `href_.c_str()`
  [[nodiscard]] const char *c_str() const noexcept {
    return href_.c_str();
  }

  /// Returns the underyling byte buffer
  ///
  /// \returns `href_.data()`
  [[nodiscard]] const char *data() const noexcept {
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
      std::optional<url_record> &&base = std::nullopt);
  void update_record(url_record &&url);

  template<class Source>
  tl::expected<void, std::error_code> set_port_impl(
      const Source &port,
      typename std::enable_if<is_url_convertible<Source>::value>::type * = nullptr) {
    auto bytes = details::to_bytes(port);
    if (!bytes) {
      return tl::make_unexpected(
          make_error_code(url_parse_errc::invalid_unicode_character));
    }

    if (!bytes.value().empty()) {
      const char *first = bytes.value().data();
      char *last = nullptr;
      auto value = std::strtol(first, &last, 10);
      if (first == last) {
        return tl::make_unexpected(
            make_error_code(url_parse_errc::invalid_port));
      }

      if (last != first + bytes.value().size()) {
        return tl::make_unexpected(
            make_error_code(url_parse_errc::invalid_port));
      }

      if ((value <= 0) ||
          (value >= static_cast<long>(std::numeric_limits<unsigned short>::max()))) {
        return tl::make_unexpected(
            make_error_code(url_parse_errc::invalid_port));
      }
    }

    return set_port(std::string_view(bytes.value()));
  }

  template<typename intT>
  tl::expected<void, std::error_code> set_port_impl(
      intT port,
      typename std::enable_if<std::is_integral<intT>::value>::type * = nullptr) {
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
void swap(url &lhs, url &rhs) noexcept;

namespace details {
tl::expected<url, std::error_code> make_url(
    url::string_view input, const std::optional<url_record> &base);
}  // namespace details

/// Parses a URL string and constructs a `url` object
///
/// \tparam Source The input string type
/// \param input The input string
/// \returns A `url` object on success, an error on failure
template<class Source>
inline tl::expected<url, std::error_code> make_url(
    const Source &input) {
  auto bytes = details::to_bytes(input);
  if (!bytes) {
    return tl::make_unexpected(
        make_error_code(url_parse_errc::invalid_unicode_character));
  }
  return details::make_url(std::string_view(bytes.value()), std::nullopt);
}

/// Parses a URL string and constructs a `url` object
///
/// \tparam Source The input string type
/// \param input The input string
/// \param base The base URL
/// \returns A `url` object on success, an error on failure
template<class Source>
inline tl::expected<url, std::error_code> make_url(
    const Source &input, const url &base) {
  auto bytes = details::to_bytes(input);
  if (!bytes) {
    return tl::make_unexpected(
        make_error_code(url_parse_errc::invalid_unicode_character));
  }
  return details::make_url(std::string_view(bytes.value()), base.record());
}

/// Tests two URLs for equality according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-equivalence)
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `true` if the `url` objects are equal, `false` otherwise
inline bool operator==(const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

/// Tests two URLs for inequality
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `!(lhs == rhs)`
inline bool operator!=(const url &lhs, const url &rhs) noexcept {
  return !(lhs == rhs);
}

/// Comparison operator
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `lhs.compare(rhs) < 0`
inline bool operator<(const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

/// Comparison operator
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `lhs.compare(rhs) > 0`
inline bool operator>(const url &lhs, const url &rhs) noexcept {
  return rhs < lhs;
}

/// Comparison operator
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `!(lhs > rhs)
inline bool operator<=(const url &lhs, const url &rhs) noexcept {
  return !(lhs > rhs);
}

/// Comparison operator
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns !(lhs < rhs)
inline bool operator>=(const url &lhs, const url &rhs) noexcept {
  return !(lhs < rhs);
}

///
/// \param os
/// \param url
/// \returns
inline std::ostream &operator<<(std::ostream &os, const url &url) {
  return os << url.href();
}

namespace literals {
/// Literal operator for a URL string
/// \param str
/// \param length
/// \return A url
inline auto operator "" _url(const char *str, std::size_t length) {
  return make_url(std::string_view(str, length));
}

///
/// \param str
/// \param length
/// \return
inline auto operator "" _url(const wchar_t *str, std::size_t length) {
  return make_url(std::wstring_view(str, length));
}

///
/// \param str
/// \param length
/// \return
inline auto operator "" _url(const char16_t *str, std::size_t length) {
  return make_url(std::u16string_view(str, length));
}

///
/// \param str
/// \param length
/// \return
inline auto operator "" _url(const char32_t *str, std::size_t length) {
  return make_url(std::u32string_view(str, length));
}
}  // namespace literals
}  // namespace v1
}  // namespace skyr

#if defined(SKYR_PLATFORM_MSVC)
#pragma warning(pop)
#endif // defined(SKYR_PLATFORM_MSVC)

#endif  // SKYR_URL_INC
