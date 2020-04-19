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
      {"file", std::nullopt},
      {"ftp", 21},
      {"http", 80},
      {"https", 443},
      {"ws", 80},
      {"wss", 443},
  };
  return schemes;
}

inline auto scheme_less(
    const default_port_list::value_type &special_scheme,
    std::string_view scheme) -> bool {
  return special_scheme.first < scheme;
};
}  // namespace details

/// \param scheme
/// \returns
inline auto is_special(std::string_view scheme) noexcept -> bool {
  const auto &schemes = details::special_schemes();
  auto it = std::lower_bound(begin(schemes), end(schemes), scheme, details::scheme_less);
  return ((it != end(schemes)) && !(scheme < it->first));
}

/// \param scheme
/// \returns
inline auto default_port(std::string_view scheme) noexcept
    -> std::optional<std::uint16_t> {
  const auto &schemes = details::special_schemes();
  auto it = std::lower_bound(begin(schemes), end(schemes), scheme, details::scheme_less);
  return ((it != end(schemes)) && !(scheme < it->first)) ? it->second : std::nullopt;
}
}  // namespace v1
}  // namespace skyr


#endif  // SKYR_URL_SCHEMES_INC
