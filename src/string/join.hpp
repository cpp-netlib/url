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
inline auto join(
    const std::vector<std::u32string> &elements, char32_t separator) {
  auto result = std::u32string();
  for (const auto &element : elements) {
    result += element + separator;
  }
  return result.substr(0, result.length() - 1);
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_JOIN_HPP
