// Copyright 2017-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_DETAILS_URL_SCHEMES_INC
#define SKYR_DETAILS_URL_SCHEMES_INC

#include <vector>
#include <utility>
#include <string>
#include <string_view>
#include <cstdint>
#include <optional>

namespace skyr {
inline namespace v1 {
namespace details {
using default_port_list = std::vector<std::pair<std::string, std::optional<std::uint16_t>>>;

/// \returns
auto special_schemes() noexcept -> const default_port_list &;

/// \param scheme
/// \returns
auto is_special(std::string_view scheme) noexcept -> bool;

/// \param scheme
/// \returns
auto default_port(std::string_view scheme) noexcept -> std::optional<std::uint16_t>;

/// \param scheme
/// \param port
/// \returns
auto is_default_port(std::string_view scheme, std::uint16_t port) noexcept -> bool;
}  // namespace details
}  // namespace v1
}  // namespace skyr


#endif  // SKYR_DETAILS_URL_SCHEMES_INC
