// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_STARTS_WITH_HPP
#define SKYR_URL_STARTS_WITH_HPP

#include <string_view>
#include <iterator>

namespace skyr {
inline namespace v1 {
inline auto starts_with(
    std::string_view input,
    std::string_view chars) noexcept {
  return (input.size() >= chars.size()) && (input.substr(0, chars.size()) == chars);
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_STARTS_WITH_HPP
