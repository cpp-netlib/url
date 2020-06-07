// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_HOSTS_HPP
#define SKYR_V1_CORE_HOSTS_HPP

#include <variant>
#include <string>
#include <skyr/v1/network/ipv4_address.hpp>
#include <skyr/v1/network/ipv6_address.hpp>

namespace skyr {
inline namespace v1 {
/// \typedef host_types
/// One of a string (for domains), an IPv4 address or an IPv6 address.
using host_types = std::variant<
    std::string,
    ipv4_address,
    ipv6_address>;

inline auto host_is_empty(const host_types &host) {
  constexpr static auto is_empty = [] (auto &&host) {
    using T = std::decay_t<decltype(host)>;

    if constexpr (std::is_same_v<T, std::string>) {
      return host.empty();
    }
    else {
      return true;
    }
  };

  return std::visit(is_empty, host);
}

inline auto host_string(const host_types &host) {
  constexpr static auto to_string = [] (auto &&host) {
    using T = std::decay_t<decltype(host)>;

    if constexpr (std::is_same_v<T, std::string>) {
      return host;
    }
    else if constexpr (std::is_same_v<T, ipv4_address>) {
      return host.serialize();
    }
    else if constexpr (std::is_same_v<T, ipv6_address>) {
      return "[" + host.serialize() + "]";
    }
  };

  return std::visit(to_string, host);
}
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_HOSTS_HPP
