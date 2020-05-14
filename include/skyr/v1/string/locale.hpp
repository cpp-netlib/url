// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_STRING_LOCALE_HPP
#define SKYR_V1_STRING_LOCALE_HPP

#include <locale>

namespace skyr {
inline namespace v1 {
template <class charT>
inline auto is_c0_control_or_whitespace(charT byte) noexcept {
  return std::iscntrl(byte, std::locale::classic()) || std::isspace(byte, std::locale::classic());
}
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_STRING_LOCALE_HPP
