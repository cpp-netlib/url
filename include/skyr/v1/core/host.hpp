// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_HOST_HPP
#define SKYR_V1_CORE_HOST_HPP

#include <variant>
#include <string>
#include <cassert>
#include <tl/expected.hpp>
#include <skyr/v1/core/errors.hpp>
#include <skyr/v1/network/ipv4_address.hpp>
#include <skyr/v1/network/ipv6_address.hpp>

namespace skyr {
inline namespace v1 {
/// Represents a domain in a [URL host](https://url.spec.whatwg.org/#host-representation)
struct domain {
  std::string name;
};

/// Represents an opaque host in a [URL host](https://url.spec.whatwg.org/#host-representation)
struct opaque_host {
  std::string name;
};

/// Represents an empty host in a [URL host](https://url.spec.whatwg.org/#host-representation)
struct empty_host {};

/// [A URL host](https://url.spec.whatwg.org/#host-representation)
class host {

  using host_types = std::variant<
      skyr::v1::ipv4_address,
      skyr::v1::ipv6_address,
      skyr::v1::domain,
      skyr::v1::opaque_host,
      skyr::v1::empty_host
      >;

 public:

  /// Constructor
  /// \param host An IPv4 address
  explicit host(skyr::v1::ipv4_address host)
      : host_(host) {}

  /// Constructor
  /// \param host An IPv6 address
  explicit host(skyr::v1::ipv6_address host)
      : host_(host) {}

  /// Constructor
  /// \param host A domain name
  explicit host(skyr::v1::domain host)
      : host_(host) {}

  /// Constructor
  /// \param host An opaque host string
  explicit host(skyr::v1::opaque_host host)
      : host_(host) {}

  /// Constructor
  /// \param hsost An empty host
  explicit host(skyr::v1::empty_host host)
      : host_(host) {}

  ///
  /// \return The host as a string
  [[nodiscard]] auto serialize() const {
    constexpr static auto serialize = [] (auto &&host) -> std::string {
      using T = std::decay_t<decltype(host)>;

      if constexpr (std::is_same_v<T, skyr::v1::ipv4_address>) {
        return host.serialize();
      }
      else if constexpr (std::is_same_v<T, skyr::v1::ipv6_address>) {
        return "[" + host.serialize() + "]";
      }
      else if constexpr (std::is_same_v<T, skyr::v1::domain> ||
                         std::is_same_v<T, skyr::v1::opaque_host>) {
        return host.name;
      }
      else {
        return std::string();
      }
    };

    return std::visit(serialize, host_);
  }

  ///
  /// \return \c true if the host is a domain, \c false otherwise
  [[nodiscard]] auto is_domain() const noexcept {
    return std::holds_alternative<skyr::v1::domain>(host_);
  }

  ///
  /// \return
  [[nodiscard]] auto domain() const noexcept -> std::optional<std::string> {
    return is_domain() ? std::make_optional(std::get<skyr::v1::domain>(host_).name) : std::nullopt;
  }

  ///
  /// \return \c true if the host is an IPv4 address, \c false otherwise
  [[nodiscard]] auto is_ipv4_address() const noexcept {
    return std::holds_alternative<skyr::v1::ipv4_address>(host_);
  }

  ///
  /// \return
  [[nodiscard]] auto ipv4_address() const noexcept {
    assert(is_ipv4_address());
    return std::get<skyr::v1::ipv4_address>(host_);
  }

  ///
  /// \return \c true if the host is an IPv6 address, \c false otherwise
  [[nodiscard]] auto is_ipv6_address() const noexcept {
    return std::holds_alternative<skyr::v1::ipv6_address>(host_);
  }

  ///
  /// \return
  [[nodiscard]] auto ipv6_address() const noexcept {
    assert(is_ipv6_address());
    return std::get<skyr::v1::ipv6_address>(host_);
  }

  ///
  /// \return \c true if the host is an opaque host, \c false otherwise
  [[nodiscard]] auto is_opaque_host() const noexcept {
    return std::holds_alternative<skyr::v1::opaque_host>(host_);
  }

  ///
  /// \return
  [[nodiscard]] auto opaque_host() const noexcept {
    return std::get<skyr::v1::opaque_host>(host_).name;
  }

  ///
  /// \return
  [[nodiscard]] auto is_empty() const noexcept {
    return std::holds_alternative<skyr::v1::empty_host>(host_);
  }

 private:

  host_types host_;
};

/// Parses a string to either a domain, IPv4 address or IPv6 address according to
/// https://url.spec.whatwg.org/#host-parsing
/// \param input An input string
/// \param is_not_special \c true to process only non-special hosts, \c false otherwise
/// \param validation_error Set to \c true if there was a validation error
/// \return A host as a domain (std::string), ipv4_address or ipv6_address, or an error code
auto parse_host(
    std::string_view input,
    bool is_not_special,
    bool *validation_error) -> tl::expected<host, url_parse_errc>;

/// Parses a string to either a domain, IPv4 address or IPv6 address according to
/// https://url.spec.whatwg.org/#host-parsing
/// Parses a string to either a domain, IPv4 address or IPv6 addess
/// \param input An input string
/// \param is_not_special \c true to process only non-special hosts, \c false otherwise
/// \return A host as a domain (std::string), ipv4_address or ipv6_address, or an error code
inline auto parse_host(
    std::string_view input,
    bool is_not_special) -> tl::expected<host, url_parse_errc> {
  [[maybe_unused]] bool validation_error = false;
  return parse_host(input, is_not_special, &validation_error);
}}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_HOST_HPP
