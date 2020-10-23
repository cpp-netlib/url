// Copyright (c) Glyn Matthews 2012-2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_UNICODE_DETAILS_TO_U8_HPP
#define SKYR_V2_UNICODE_DETAILS_TO_U8_HPP

#include <string>
#include <skyr/v2/concepts/url_concepts.hpp>
#include <skyr/v2/unicode/ranges/transforms/u8_transform.hpp>
#include <skyr/v2/unicode/ranges/views/u16_view.hpp>
#include <skyr/v2/unicode/errors.hpp>

namespace skyr::inline v2::details {
template <class Source> requires is_string_container<Source, char>
inline auto to_u8(const Source &source)
    -> tl::expected<std::string, unicode::unicode_errc> {
  return std::string(source);
}

template <class Source> requires is_string_container<Source, char8_t>
inline auto to_u8(const Source &source)
    -> tl::expected<std::string, unicode::unicode_errc> {
  return std::string(std::cbegin(source), std::cend(source));
}

template <class Source> requires is_string_container<Source, char16_t> || is_string_container<Source, wchar_t>
inline auto to_u8(const Source &source)
    -> tl::expected<std::string, unicode::unicode_errc> {
  return unicode::as<std::string>(unicode::views::as_u16(source) | unicode::transforms::to_u8);
}

template <class Source> requires is_string_container<Source, char32_t>
inline auto to_u8(const Source &source)
    -> tl::expected<std::string, unicode::unicode_errc> {
  return unicode::as<std::string>(source | unicode::transforms::to_u8);
}
}  // namespace skyr::inline v2::details

#endif  // SKYR_V2_UNICODE_DETAILS_TO_U8_HPP
