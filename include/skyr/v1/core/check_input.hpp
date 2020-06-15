// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_CHECK_INPUT_HPP
#define SKYR_V1_CORE_CHECK_INPUT_HPP

#include <locale>
#include <string>

namespace skyr {
inline namespace v1 {
constexpr static auto is_c0_control_or_space = [] (auto byte) {
  return std::iscntrl(byte, std::locale::classic()) || std::isspace(byte, std::locale::classic());
};

inline auto remove_leading_c0_control_or_space(std::string_view input, bool *validation_error) {
  auto first = begin(input), last = end(input);
  auto it = std::find_if_not(first, last, is_c0_control_or_space);
  *validation_error |= (it != first);
  input.remove_prefix(std::distance(first, it));
  return input;
}

inline auto remove_trailing_c0_control_or_space(std::string_view input, bool *validation_error) {
  auto first = rbegin(input), last = rend(input);
  auto it = std::find_if_not(first, last, is_c0_control_or_space);
  *validation_error |= (it != first);
  input.remove_suffix(std::distance(first, it));
  return input;
}

inline auto remove_tabs_and_newlines(std::string &input, bool *validation_error) {
  constexpr static auto is_tab_or_newline = [] (auto byte) {
    return (byte == '\t') || (byte == '\r') || (byte == '\n');
  };

  auto first = begin(input), last = end(input);
  auto it = std::remove_if(first, last, is_tab_or_newline);
  *validation_error |= (it != last);
  input.erase(it, last);
}
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_CHECK_INPUT_HPP
