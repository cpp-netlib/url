// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_ALGORITHMS_HPP
#define SKYR_ALGORITHMS_HPP

#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include <locale>

namespace skyr {
inline bool is_ascii(std::u32string_view input) noexcept {
  auto first = begin(input), last = end(input);
  auto it = std::find_if(
      first, last,
      [] (auto c) -> bool {
        return c > 0x007e;
      });
  return it == last;
}

inline std::vector<std::u32string> split(
    std::u32string_view input, char32_t separator) noexcept {
  auto elements = std::vector<std::u32string>{};
  if (!input.empty()) {
    auto first = begin(input), last = end(input);
    auto it = first;
    auto prev = it;
    while (it != last) {
      if (*it == separator) {
        elements.emplace_back(prev, it);
        ++it;
        prev = it;
      } else {
        ++it;
      }
    }
    elements.emplace_back(prev, it);
  }
  return elements;
}

inline std::u32string join(
    const std::vector<std::u32string> &elements, char32_t separator) {
  auto result = std::u32string();
  for (const auto &element : elements) {
    result += element + separator;
  }
  return result.substr(0, result.length() - 1);
}

inline bool starts_with(
    std::string_view::const_iterator first,
    std::string_view::const_iterator last,
    const char *chars) noexcept {
  auto chars_view = std::string_view(chars);
  auto chars_first = std::begin(chars_view), chars_last = std::end(chars_view);
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

inline bool is_in(
    std::string_view::value_type byte,
    std::string_view view) noexcept {
  auto first = begin(view), last = end(view);
  return last != std::find(first, last, byte);
}

inline bool is_c0_control_or_whitespace(
    char byte,
    const std::locale &locale = std::locale::classic()) noexcept {
  return std::iscntrl(byte, locale) || std::isspace(byte, locale);
}

inline bool remove_leading_whitespace(std::string &input) {
  auto view = std::string_view(input);
  auto first = begin(view), last = end(view);
  auto it = std::find_if(
      first, last,
      [] (auto byte) -> bool {
        return !is_c0_control_or_whitespace(byte);
      });
  if (it != first) {
    input.assign(it, last);
  }

  return it == first;
}

inline bool remove_trailing_whitespace(std::string &input) {
  using reverse_iterator =
      std::reverse_iterator<std::string::const_iterator>;

  auto first = reverse_iterator(end(input)),
      last = reverse_iterator(begin(input));
  auto it = std::find_if(
      first, last,
      [] (auto byte) -> bool {
        return !is_c0_control_or_whitespace(byte);
      });
  if (it != first) {
    input = std::string(it, last);
    std::reverse(begin(input), end(input));
  }

  return it == first;
}
}  // namespace skyr

#endif //SKYR_ALGORITHMS_HPP
