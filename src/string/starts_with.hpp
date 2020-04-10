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
inline bool starts_with(
    std::string_view input,
    std::string_view chars) noexcept {
  auto first = std::begin(input), last = std::end(input);
  auto chars_first = std::begin(chars), chars_last = std::end(chars);
  auto chars_it = chars_first;
  auto it = first;
  if (it == last) {
    return false;
  }

  while (chars_it != chars_last) {
    if (*it != *chars_it) {
      return false;
    }

    ++it;
    ++chars_it;

    if (it == last) {
      return (chars_it == chars_last);
    }
  }

  return true;
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_STARTS_WITH_HPP
