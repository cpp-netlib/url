// Copyright (c) Glyn Matthews 2012-2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_DETAILS_TO_U8_HPP
#define SKYR_UNICODE_DETAILS_TO_U8_HPP

#include <string>

#include <skyr/concepts/url_concepts.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/ranges/transforms/u8_transform.hpp>
#include <skyr/unicode/ranges/views/u16_view.hpp>

namespace skyr::details {
template <class Source>
  requires is_string_container<Source, char>
auto to_u8(const Source& source) -> std::expected<std::string, unicode::unicode_errc> {
  // For char arrays/pointers (null-terminated), find the actual end excluding null terminator
  auto begin = std::cbegin(source);
  auto end = std::cend(source);

  // If this is a null-terminated string, exclude the null terminator
  if constexpr (is_char_array<Source, char> || is_char_pointer<Source, char>) {
    // Find the actual end (before null terminator)
    while (begin != end && *(end - 1) == char{0}) {
      --end;
    }
  }

  return std::string(begin, end);
}

template <class Source>
  requires is_string_container<Source, char8_t>
auto to_u8(const Source& source) -> std::expected<std::string, unicode::unicode_errc> {
  // For char8_t arrays/pointers (null-terminated), find the actual end excluding null terminator
  auto begin = std::cbegin(source);
  auto end = std::cend(source);

  // If this is a null-terminated string, exclude the null terminator
  if constexpr (is_char_array<Source, char8_t> || is_char_pointer<Source, char8_t>) {
    // Find the actual end (before null terminator)
    while (begin != end && *(end - 1) == char8_t{0}) {
      --end;
    }
  }

  return std::string(begin, end);
}

template <class Source>
  requires is_string_container<Source, char16_t> || is_string_container<Source, wchar_t>
inline auto to_u8(const Source& source) -> std::expected<std::string, unicode::unicode_errc> {
  return unicode::as<std::string>(unicode::views::as_u16(source) | unicode::transforms::to_u8);
}

template <class Source>
  requires is_string_container<Source, char32_t>
inline auto to_u8(const Source& source) -> std::expected<std::string, unicode::unicode_errc> {
  return unicode::as<std::string>(source | unicode::transforms::to_u8);
}
}  // namespace skyr::details

#endif  // SKYR_UNICODE_DETAILS_TO_U8_HPP
