// Copyright 2017-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_CORE_URL_SCHEMES_HPP
#define SKYR_V2_CORE_URL_SCHEMES_HPP

#include <string_view>
#include <cstdint>
#include <optional>

namespace skyr::inline v2 {
/// \param scheme
/// \returns
constexpr inline auto is_special(std::string_view scheme) noexcept -> bool {
  return (scheme == "file") || (scheme == "ftp") || (scheme == "http") || (scheme == "https") || (scheme == "ws") ||
         (scheme == "wss");
}

/// \param scheme
/// \returns
constexpr inline auto default_port(std::string_view scheme) noexcept -> std::optional<std::uint16_t> {
  if (scheme == "ftp") {
    return 21;
  } else if ((scheme == "http") || (scheme == "ws")) {
    return 80;
  } else if ((scheme == "https") || (scheme == "wss")) {
    return 443;
  }
  return std::nullopt;
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_CORE_URL_SCHEMES_HPP
