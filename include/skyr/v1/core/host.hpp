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
///
class host {

  using host_types = std::variant<
      std::string,
      skyr::v1::ipv4_address,
      skyr::v1::ipv6_address>;

 public:

  /// Constructor
  /// \param host An ASCII string that can be either a domain,
  ///        opaque host or empty host
  explicit host(std::string host)
      : host_(host) {}

  /// Constructor
  /// \param host An IPv4 address
  explicit host(skyr::v1::ipv4_address host)
      : host_(host) {}

  /// Constructor
  /// \param host An IPv6 address
  explicit host(skyr::v1::ipv6_address host)
      : host_(host) {}

  ///
  /// \return
  [[nodiscard]] auto is_empty() const noexcept {
    constexpr static auto is_empty = [] (auto &&host) {
      using T = std::decay_t<decltype(host)>;

      if constexpr (std::is_same_v<T, std::string>) {
        return host.empty();
      }
      else {
        return true;
      }
    };

    return std::visit(is_empty, host_);
  }

  ///
  /// \return The host as a string
  [[nodiscard]] auto serialize() const {
    constexpr static auto serialize = [] (auto &&host) {
      using T = std::decay_t<decltype(host)>;

      if constexpr (std::is_same_v<T, std::string>) {
        return host;
      }
      else if constexpr (std::is_same_v<T, skyr::v1::ipv4_address>) {
        return host.serialize();
      }
      else if constexpr (std::is_same_v<T, skyr::v1::ipv6_address>) {
        return "[" + host.serialize() + "]";
      }
    };

    return std::visit(serialize, host_);
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
auto parse_host(
    std::string_view input,
    bool is_not_special) -> tl::expected<host, url_parse_errc>;
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_HOST_HPP
