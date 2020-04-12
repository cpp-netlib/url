// Copyright 2017-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include "url_schemes.hpp"

namespace skyr {
inline namespace v1 {
namespace details {
auto special_schemes() noexcept -> const default_port_list & {
  static const auto schemes = default_port_list{
    {"ftp", 21},
    {"file", std::nullopt},
    {"http", 80},
    {"https", 443},
    {"ws", 80},
    {"wss", 443},
    };
  return schemes;
}

auto default_port(std::string_view scheme) noexcept -> std::optional<std::uint16_t> {
  auto schemes = special_schemes();
  auto first = begin(schemes), last = end(schemes);
  auto it = std::find_if(
      first, last,
      [&scheme](const auto &special_scheme) -> bool {
        return scheme == special_scheme.first;
      });
  if (it != last) {
    return it->second;
  }
  return std::nullopt;
}

auto is_special(std::string_view scheme) noexcept -> bool {
  auto schemes = special_schemes();
  auto first = begin(schemes), last = end(schemes);
  auto it = std::find_if(
      first, last,
      [&scheme](const auto &special_scheme) -> bool {
        return scheme == special_scheme.first;
      });
  return (it != last);
}

auto is_default_port(std::string_view scheme, std::uint16_t port) noexcept -> bool {
  auto dport = default_port(scheme);
  return dport && (dport.value() == port);
}
}  // namespace details
}  // namespace v1
}  // namespace skyr
