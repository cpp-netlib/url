// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_ASCII_HPP
#define SKYR_URL_ASCII_HPP

#include <string_view>
#include <iterator>
#include <algorithm>

namespace skyr {
inline namespace v1 {
template <class charT>
inline auto is_ascii(std::basic_string_view<charT> input) noexcept {
  auto first = begin(input), last = end(input);
  return last == std::find_if(first, last, [](auto c) { return c > static_cast<charT>(0x7eu); });
}

template <class charT>
inline auto is_ascii(const std::basic_string<charT> &input) noexcept {
  return is_ascii(std::basic_string_view<charT>(input));
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_ASCII_HPP
