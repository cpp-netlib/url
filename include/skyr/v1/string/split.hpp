// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_STRING_SPLIT_HPP
#define SKYR_V1_STRING_SPLIT_HPP

#include <string_view>
#include <skyr/v1/ranges/string_element_range.hpp>

namespace skyr {
inline namespace v1 {
///
/// \tparam charT
/// \param s
/// \param separators
/// \returns A lazy range of string_views, delimited by any character in `separators`
template <class charT>
inline auto split(std::basic_string_view<charT> s, decltype(s) separators)
-> string_element_range<charT> {
  return { s, separators };
}
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_STRING_SPLIT_HPP
