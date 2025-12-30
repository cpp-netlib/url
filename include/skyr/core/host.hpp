// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_CORE_HOST_HPP
#define SKYR_CORE_HOST_HPP

#include <algorithm>
#include <cassert>
#include <expected>
#include <ranges>
#include <string>
#include <variant>

#include <skyr/core/errors.hpp>
#include <skyr/domain/domain.hpp>
#include <skyr/network/ipv4_address.hpp>
#include <skyr/network/ipv6_address.hpp>
#include <skyr/percent_encoding/percent_decode.hpp>
#include <skyr/percent_encoding/percent_encoded_char.hpp>

namespace skyr {
/// Represents a domain name in a [URL host](https://url.spec.whatwg.org/#host-representation)
struct domain_name {
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
  using host_types = std::variant<ipv4_address, ipv6_address, domain_name, opaque_host, empty_host>;

 public:
  /// Constructor
  /// \param host An IPv4 address
  constexpr explicit host(ipv4_address host) : host_(host) {
  }

  /// Constructor
  /// \param host An IPv6 address
  constexpr explicit host(ipv6_address host) : host_(host) {
  }

  /// Constructor
  /// \param host A domain name
  explicit host(domain_name host) : host_(std::move(host)) {
  }

  /// Constructor
  /// \param host An opaque host string
  explicit host(opaque_host host) : host_(std::move(host)) {
  }

  /// Constructor
  /// \param hsost An empty host
  constexpr explicit host(empty_host host) : host_(host) {
  }

  ///
  /// \return The host as a string
  [[nodiscard]] auto serialize() const {
    constexpr static auto serialize = [](auto&& host) -> std::string {
      using T = std::decay_t<decltype(host)>;

      if constexpr (std::is_same_v<T, ipv4_address>) {
        return host.serialize();
      } else if constexpr (std::is_same_v<T, ipv6_address>) {
        return std::format("[{}]", host.serialize());
      } else if constexpr (std::is_same_v<T, domain_name> || std::is_same_v<T, opaque_host>) {
        return host.name;
      } else {
        return std::string();
      }
    };

    return std::visit(serialize, host_);
  }

  ///
  /// \return \c true if the host is a domain, \c false otherwise
  [[nodiscard]] constexpr auto is_domain_name() const noexcept {
    return std::holds_alternative<domain_name>(host_);
  }

  ///
  /// \return
  [[nodiscard]] auto to_domain_name() const noexcept -> std::optional<std::string> {
    return is_domain_name() ? std::make_optional(std::get<domain_name>(host_).name) : std::nullopt;
  }

  ///
  /// \return \c true if the host is an IPv4 address, \c false otherwise
  [[nodiscard]] constexpr auto is_ipv4_address() const noexcept {
    return std::holds_alternative<ipv4_address>(host_);
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto to_ipv4_address() const noexcept {
    return is_ipv4_address() ? std::make_optional(std::get<ipv4_address>(host_)) : std::nullopt;
  }

  ///
  /// \return \c true if the host is an IPv6 address, \c false otherwise
  [[nodiscard]] constexpr auto is_ipv6_address() const noexcept {
    return std::holds_alternative<ipv6_address>(host_);
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto to_ipv6_address() const noexcept {
    return is_ipv6_address() ? std::make_optional(std::get<ipv6_address>(host_)) : std::nullopt;
  }

  ///
  /// \return \c true if the host is an opaque host, \c false otherwise
  [[nodiscard]] constexpr auto is_opaque_host() const noexcept {
    return std::holds_alternative<opaque_host>(host_);
  }

  ///
  /// \return
  [[nodiscard]] auto to_opaque_host() const noexcept {
    return is_opaque_host() ? std::make_optional(std::get<opaque_host>(host_).name) : std::nullopt;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto is_empty() const noexcept {
    return std::holds_alternative<empty_host>(host_);
  }

 private:
  host_types host_;
};

namespace details {
constexpr static auto is_forbidden_host_point = [](auto byte) {
  return (byte == '\0') || (byte == '\t') || (byte == '\n') || (byte == '\r') || (byte == ' ') || (byte == '#') ||
         (byte == '%') || (byte == '/') || (byte == ':') || (byte == '<') || (byte == '>') || (byte == '?') ||
         (byte == '@') || (byte == '[') || (byte == '\\') || (byte == ']') || (byte == '^');
};

inline auto parse_opaque_host(std::string_view input, bool* validation_error)
    -> std::expected<opaque_host, url_parse_errc> {
  constexpr auto is_forbidden = [](auto byte) -> bool { return (byte != '%') && is_forbidden_host_point(byte); };

  constexpr auto pct_encode = [](auto c) {
    return percent_encode_byte(std::byte(c), percent_encoding::encode_set::c0_control).to_string();
  };

  auto it = std::ranges::find_if(input, is_forbidden);
  if (it != std::cend(input)) {
    *validation_error |= true;
    return std::unexpected(url_parse_errc::forbidden_host_point);
  }

  std::string result;
  for (auto c : input) {
    result += pct_encode(c);
  }
  return opaque_host{std::move(result)};
}
}  // namespace details

/// Parses a string to either a domain, IPv4 address or IPv6 address according to
/// https://url.spec.whatwg.org/#host-parsing
/// \param input An input string
/// \param is_not_special \c true to process only non-special hosts, \c false otherwise
/// \param validation_error Set to \c true if there was a validation error
/// \return A host as a domain (std::string), ipv4_address or ipv6_address, or an error code
inline auto parse_host(std::string_view input, bool is_not_special, bool* validation_error)
    -> std::expected<host, url_parse_errc> {
  if (input.empty()) {
    return host{empty_host{}};
  }

  if (input.front() == '[') {
    if (input.back() != ']') {
      *validation_error |= true;
      return std::unexpected(url_parse_errc::invalid_ipv6_address);
    }

    auto view = std::string_view(input);
    view.remove_prefix(1);
    view.remove_suffix(1);
    bool ipv6_validation_error = false;
    auto ipv6_address = parse_ipv6_address(view, &ipv6_validation_error);
    if (ipv6_address) {
      *validation_error = ipv6_validation_error;
      return host{ipv6_address.value()};
    } else {
      return std::unexpected(url_parse_errc::invalid_ipv6_address);
    }
  }

  if (is_not_special) {
    return details::parse_opaque_host(input, validation_error)
        .and_then([](auto&& h) -> std::expected<host, url_parse_errc> { return host{h}; });
  }

  auto decoded_domain = std::string{};
  auto range = percent_encoding::percent_decode_range{input};
  for (auto it = std::cbegin(range); it != std::cend(range); ++it) {
    if (!*it) {
      return std::unexpected(url_parse_errc::cannot_decode_host_point);
    }
    decoded_domain.push_back((*it).value());
  }

  auto ascii_domain = std::string{};
  if (!domain_to_ascii(decoded_domain, &ascii_domain)) {
    return std::unexpected(url_parse_errc::domain_error);
  }

  auto it = std::ranges::find_if(ascii_domain, details::is_forbidden_host_point);
  if (std::cend(ascii_domain) != it) {
    *validation_error |= true;
    return std::unexpected(url_parse_errc::domain_error);
  }

  bool ipv4_validation_error = false;
  auto ipv4_result = parse_ipv4_address(ascii_domain, &ipv4_validation_error);
  if (!ipv4_result) {
    if (ipv4_result.error() == ipv4_address_errc::overflow) {
      return std::unexpected(url_parse_errc::invalid_ipv4_address);
    } else {
      return host{domain_name{std::move(ascii_domain)}};
    }
  }
  *validation_error = ipv4_validation_error;
  return host{ipv4_result.value()};
}

/// Parses a string to either a domain, IPv4 address or IPv6 address according to
/// https://url.spec.whatwg.org/#host-parsing
/// Parses a string to either a domain, IPv4 address or IPv6 addess
/// \param input An input string
/// \param is_not_special \c true to process only non-special hosts, \c false otherwise
/// \return A host as a domain (std::string), ipv4_address or ipv6_address, or an error code
inline auto parse_host(std::string_view input, bool is_not_special) -> std::expected<host, url_parse_errc> {
  [[maybe_unused]] bool validation_error = false;
  return parse_host(input, is_not_special, &validation_error);
}

/// Parses a string to either a domain, IPv4 address or IPv6 address according to
/// https://url.spec.whatwg.org/#host-parsing
/// Parses a string to either a domain, IPv4 address or IPv6 addess
/// \param input An input string
/// \return A host as a domain (std::string), ipv4_address or ipv6_address, or an error code
inline auto parse_host(std::string_view input) -> std::expected<host, url_parse_errc> {
  [[maybe_unused]] bool validation_error = false;
  return parse_host(input, false, &validation_error);
}

/// Parses a string to either a domain, IPv4 address or IPv6 address according to
/// https://url.spec.whatwg.org/#host-parsing
/// Parses a string to either a domain, IPv4 address or IPv6 addess
/// \param input An input string
/// \param validation_error Set to \c true if there was a validation error
/// \return A host as a domain (std::string), ipv4_address or ipv6_address, or an error code
inline auto parse_host(std::string_view input, bool* validation_error) -> std::expected<host, url_parse_errc> {
  return parse_host(input, false, validation_error);
}
}  // namespace skyr

#endif  // SKYR_CORE_HOST_HPP
