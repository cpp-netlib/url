// Copyright (c) Glyn Matthews 2018-20.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_CONCEPTS_URL_CONCEPTS_HPP
#define SKYR_V2_CONCEPTS_URL_CONCEPTS_HPP

#include <type_traits>
#include <string>
#include <string_view>

namespace skyr::inline v2 {
template <class T, class charT>
concept is_basic_string = std::is_same_v<std::remove_cv_t<T>, std::basic_string<charT>>;

template <class T, class charT>
concept is_basic_string_view = std::is_same_v<std::remove_cv_t<T>, std::basic_string_view<charT>>;

template <class T, class charT>
concept is_char_array =
    std::conjunction_v<std::is_array<T>, std::is_same<std::remove_cv_t<std::remove_extent_t<T>>, charT>>;

template <class T, class charT>
concept is_char_pointer = std::conjunction_v<std::is_pointer<T>, std::is_same<std::remove_pointer_t<T>, charT>>;

template <class T, class charT>
concept is_string_container =
    is_basic_string<T, charT> || is_basic_string_view<T, charT> || is_char_array<T, charT> || is_char_pointer<T, charT>;

template <typename T>
concept is_u8_convertible =
    is_string_container<T, char> || is_string_container<T, char8_t> || is_string_container<T, wchar_t> ||
    is_string_container<T, char16_t> || is_string_container<T, char32_t>;
}  // namespace skyr::inline v2

#endif  // SKYR_V2_CONCEPTS_URL_CONCEPTS_HPP
