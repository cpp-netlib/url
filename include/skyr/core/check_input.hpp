// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_CORE_CHECK_INPUT_HPP
#define SKYR_CORE_CHECK_INPUT_HPP

#include <algorithm>
#include <iterator>
#include <locale>
#include <string>

namespace skyr {
constexpr static auto is_c0_control_or_space = [](auto byte) {
  return std::iscntrl(byte, std::locale::classic()) || std::isspace(byte, std::locale::classic());
};

constexpr inline auto remove_leading_c0_control_or_space(std::string_view input, bool* validation_error) {
  auto first = std::cbegin(input), last = std::cend(input);
  auto it = std::find_if_not(first, last, is_c0_control_or_space);
  *validation_error |= (it != first);
  input.remove_prefix(std::distance(first, it));
  return input;
}

constexpr inline auto remove_trailing_c0_control_or_space(std::string_view input, bool* validation_error) {
  auto first = std::crbegin(input), last = std::crend(input);
  auto it = std::find_if_not(first, last, is_c0_control_or_space);
  *validation_error |= (it != first);
  input.remove_suffix(std::distance(first, it));
  return input;
}

constexpr static auto is_tab_or_newline = [](auto byte) { return (byte == '\t') || (byte == '\r') || (byte == '\n'); };

inline auto remove_tabs_and_newlines(std::string_view input, bool* validation_error) -> std::string {
  std::string result;
  result.reserve(input.size());

  for (auto byte : input) {
    if (is_tab_or_newline(byte)) {
      *validation_error = true;
    } else {
      result.push_back(byte);
    }
  }

  return result;
}
}  // namespace skyr

#endif  // SKYR_CORE_CHECK_INPUT_HPP
