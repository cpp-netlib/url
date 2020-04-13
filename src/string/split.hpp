// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_SPLIT_HPP
#define SKYR_URL_SPLIT_HPP

#include <string_view>
#include <string>
#include <vector>
#include <iterator>

namespace skyr {
inline namespace v1 {
template <class charT>
inline auto split(
    std::basic_string_view<charT> input, charT separator) noexcept {
  auto elements = std::vector<std::basic_string<charT>>{};
  if (!input.empty()) {
    auto it = begin(input), prev = begin(input), last = end(input);
    while (it != last) {
      if (*it == separator) {
        elements.emplace_back(prev, it);
        prev = ++it;
      } else {
        ++it;
      }
    }
    elements.emplace_back(prev, it);
  }
  return elements;
}

template <class charT>
inline auto split(
    const std::basic_string<charT> &input, charT separator) noexcept {
  return split(std::basic_string_view<charT>(input), separator);
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_SPLIT_HPP
