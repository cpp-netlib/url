// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_STRING_ASCII_HPP
#define SKYR_V1_STRING_ASCII_HPP

#include <string_view>
#include <iterator>
#include <algorithm>
#include <locale>
#include <cassert>

namespace skyr {
inline namespace v1 {
template <class charT>
inline auto is_ascii(std::basic_string_view<charT> input) noexcept {
  constexpr static auto is_not_in_ascii_set = [](charT c) { return c > static_cast<charT>(0x7eu); };

  auto first = cbegin(input), last = cend(input);
  return last == std::find_if(first, last, is_not_in_ascii_set);
}

template <class charT>
inline auto is_ascii(const std::basic_string<charT> &input) noexcept {
  return is_ascii(std::basic_string_view<charT>(input));
}

template <class intT, class charT>
inline auto hex_to_dec(charT byte) noexcept {
  assert(std::isxdigit(byte, std::locale::classic()));

  if (std::isdigit(byte, std::locale::classic())) {
    return static_cast<intT>(byte - '0');
  }

  return static_cast<intT>(std::tolower(byte, std::locale::classic()) - 'a' + 10);
}
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_STRING_ASCII_HPP
