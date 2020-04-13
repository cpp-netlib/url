// Copyright 2017-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_SCHEMES_INC
#define SKYR_URL_SCHEMES_INC

#include <vector>
#include <utility>
#include <string>
#include <string_view>
#include <cstdint>
#include <optional>
#include <algorithm>

namespace skyr {
inline namespace v1 {
namespace details {
using default_port_list = std::vector<std::pair<std::string, std::optional<std::uint16_t>>>;

inline auto special_schemes() noexcept -> const default_port_list & {
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
}  // namespace details

/// \param scheme
/// \returns
inline auto is_special(std::string_view scheme) noexcept -> bool {
  const auto &schemes = details::special_schemes();
  auto first = begin(schemes), last = end(schemes);
  auto it = std::find_if(
      first, last,
      [&scheme](const auto &special_scheme) -> bool {
        return scheme == special_scheme.first;
      });
  return (it != last);
}

/// \param scheme
/// \returns
inline auto default_port(std::string_view scheme) noexcept -> std::optional<std::uint16_t> {
  const auto &schemes = details::special_schemes();
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
}  // namespace v1
}  // namespace skyr


#endif  // SKYR_URL_SCHEMES_INC
