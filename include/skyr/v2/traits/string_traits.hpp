// Copyright (c) Glyn Matthews 2018-19.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_STRING_TRAITS_HPP
#define SKYR_V2_STRING_TRAITS_HPP

#include <type_traits>
#include <string>
#include <string_view>

namespace skyr {
inline namespace v2 {
//template <class charT>
//concept is_basic_string = std::is_same<
//    std::remove_cv_t<T>,
//    std::basic_string<charT>>;

    /// Meta-function to test if the type is of the form
/// basic_string<charT>
template <class T, class charT>
using is_basic_string =
  std::is_same<
      std::remove_cv_t<T>,
      std::basic_string<charT>>;

template <class T, class charT>
concept is_basic_string_c = std::is_same<
    std::remove_cv_t<T>,
    std::basic_string<charT>>::value;


/// Meta-function to test if the type is of the form
/// basic_string_view<charT>
template <class T, class charT>
using is_basic_string_view =
  std::is_same<
      std::remove_cv_t<T>,
      std::basic_string_view<charT>>;

template <class T, class charT>
concept is_basic_string_view_c =
  std::is_same<
      std::remove_cv_t<T>,
      std::basic_string_view<charT>>::value;

/// Meta-function to test if the type is of the form charT[]
template <class T, class charT>
using is_char_array =
  std::conjunction<
    std::is_array<T>,
    std::is_same<
        std::remove_cv_t<std::remove_extent_t<T>>,
        charT>>;

template <class T, class charT>
concept is_char_array_c =
  std::conjunction<
    std::is_array<T>,
    std::is_same<
        std::remove_cv_t<std::remove_extent_t<T>>,
        charT>>::value;

/// Meta-function to test if the type is of the form charT*
template <class T, class charT>
using is_char_pointer =
  std::conjunction<
    std::is_pointer<T>,
    std::is_same<
        std::remove_pointer_t<T>,
        charT>>;

template <class T, class charT>
concept is_char_pointer_c =
  std::conjunction<
    std::is_pointer<T>,
    std::is_same<
        std::remove_pointer_t<T>,
        charT>>::value;

/// Meta-function to test if the type can be converted to a
/// basic_string<charT>
template <class T, class charT>
using is_string_convertible =
  std::disjunction<
      is_basic_string<T, charT>,
      is_basic_string_view<T, charT>,
      is_char_array<T, charT>,
      is_char_pointer<T, charT>>
  ;

template <class T, class charT>
inline constexpr auto is_string_convertible_v = is_string_convertible<T, charT>::value;

/// Meta-function to test if it can be used to construct a url
template <class T>
using is_url_convertible =
  std::disjunction<
      is_string_convertible<T, char>,
      is_string_convertible<T, char8_t>,
      is_string_convertible<T, wchar_t>,
      is_string_convertible<T, char16_t>,
      is_string_convertible<T, char32_t>>;

template <class T>
inline constexpr auto is_url_convertible_v = is_url_convertible<T>::value;

template <typename T>
concept is_url_convertible_c =
    is_string_convertible_v<T, char> ||
    is_string_convertible_v<T, char8_t> ||
    is_string_convertible_v<T, wchar_t> ||
    is_string_convertible_v<T, char16_t> ||
    is_string_convertible_v<T, char32_t>
        ;
}  // namespace v2
}  // namespace skyr

#endif //SKYR_V2_STRING_TRAITS_HPP
