// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_STRING_STARTS_WITH_HPP
#define SKYR_V1_STRING_STARTS_WITH_HPP

#include <string_view>
#include <iterator>

namespace skyr {
inline namespace v1 {
template <class charT>
inline auto starts_with(
    std::basic_string_view<charT> input,
    decltype(input) chars) noexcept {
  return (input.size() >= chars.size()) && (input.substr(0, chars.size()) == chars);
}
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_STRING_STARTS_WITH_HPP
