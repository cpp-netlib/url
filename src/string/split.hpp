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
inline auto split(
    std::u32string_view input, char32_t separator) noexcept {
  auto elements = std::vector<std::u32string>{};
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
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_SPLIT_HPP
