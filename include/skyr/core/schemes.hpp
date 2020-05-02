// Copyright 2017-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_SCHEMES_HPP
#define SKYR_URL_SCHEMES_HPP

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
using namespace std::string_view_literals;

using default_port_list = std::array<std::pair<std::string_view, std::optional<std::uint16_t>>, 6>;
constexpr static auto schemes = default_port_list{{
                                               {"file"sv, std::nullopt},
                                               {"ftp"sv, 21},
                                               {"http"sv, 80},
                                               {"https"sv, 443},
                                               {"ws"sv, 80},
                                               {"wss"sv, 443},
                                           }};


constexpr auto scheme_less(
    const default_port_list::value_type &special_scheme,
    std::string_view scheme) {
  return special_scheme.first < scheme;
};
}  // namespace details

/// \param scheme
/// \returns
inline auto is_special(std::string_view scheme) noexcept {
  auto it = std::lower_bound(cbegin(details::schemes), cend(details::schemes), scheme, details::scheme_less);
  return ((it != end(details::schemes)) && !(scheme < it->first));
}

/// \param scheme
/// \returns
inline auto default_port(std::string_view scheme) noexcept {
  auto it = std::lower_bound(cbegin(details::schemes), cend(details::schemes), scheme, details::scheme_less);
  return ((it != end(details::schemes)) && !(scheme < it->first)) ? it->second : std::nullopt;
}
}  // namespace v1
}  // namespace skyr


#endif  // SKYR_URL_SCHEMES_HPP
