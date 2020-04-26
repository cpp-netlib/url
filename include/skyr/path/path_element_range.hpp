// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PATH_RANGE_HPP
#define SKYR_URL_PATH_RANGE_HPP

#include <skyr/ranges/string_element_range.hpp>

namespace skyr {
inline namespace v1 {
constexpr static auto path_separator = [] (auto c) { return (c == '/') || (c == '\\'); };
using path_element_range = string_element_range<char, decltype(path_separator)>;
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_PATH_RANGE_HPP
