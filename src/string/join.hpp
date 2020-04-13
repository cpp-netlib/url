// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_JOIN_HPP
#define SKYR_URL_JOIN_HPP

#include <string_view>
#include <string>
#include <vector>

namespace skyr {
inline namespace v1 {
template <class charT>
inline auto join(
    const std::vector<std::basic_string<charT>> &elements, charT separator) {
  auto result = std::basic_string<charT>();
  for (const auto &element : elements) {
    result += element + separator;
  }
  return result.substr(0, result.length() - 1);
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_JOIN_HPP
