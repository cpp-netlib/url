// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_CHECK_INPUT_HPP
#define SKYR_V1_CORE_CHECK_INPUT_HPP

#include <locale>
#include <string>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/action/erase.hpp>

namespace skyr {
inline namespace v1 {
constexpr static auto is_c0_control_or_space = [] (auto byte) {
  return std::iscntrl(byte, std::locale::classic()) || std::isspace(byte, std::locale::classic());
};

inline auto remove_leading_c0_control_or_space(std::string_view input, bool *validation_error) {
  auto it = ranges::find_if_not(input, is_c0_control_or_space);
  *validation_error |= (it != ranges::cbegin(input));
  input.remove_prefix(std::distance(ranges::cbegin(input), it));
  return input;
}

inline auto remove_trailing_c0_control_or_space(std::string_view input, bool *validation_error) {
  auto reversed = ranges::reverse_view(input);
  auto it = ranges::find_if_not(reversed, is_c0_control_or_space);
  *validation_error |= (it != ranges::cbegin(reversed));
  input.remove_suffix(std::distance(ranges::cbegin(reversed), it));
  return input;
}

inline auto remove_tabs_and_newlines(std::string &input, bool *validation_error) {
  constexpr static auto is_tab_or_newline = [] (auto byte) {
    return (byte == '\t') || (byte == '\r') || (byte == '\n');
  };

  auto it = ranges::remove_if(input, is_tab_or_newline);
  *validation_error |= (it != std::cend(input));
  ranges::erase(input, it, std::cend(input));
}
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_CHECK_INPUT_HPP
