// Copyright (c) Glyn Matthews 2012-2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_DETAILS_TO_U8_HPP
#define SKYR_V1_UNICODE_DETAILS_TO_U8_HPP

#include <string>
#include <skyr/v1/traits/string_traits.hpp>
#include <skyr/v1/unicode/ranges/transforms/u8_transform.hpp>
#include <skyr/v1/unicode/ranges/views/u16_view.hpp>
#include <skyr/v1/unicode/errors.hpp>

namespace skyr {
inline namespace v1 {
namespace details {
template<class Source, class Enable = void>
struct to_u8_impl;

template<class Source>
struct to_u8_impl<
    Source, std::enable_if_t<is_string_convertible_v<Source, char>>> {
  auto operator()(const Source &source) -> tl::expected<std::string, unicode::unicode_errc> {
    return std::string(source);
  }
};

template<class Source>
struct to_u8_impl<
    Source, std::enable_if_t<is_string_convertible_v<Source, wchar_t>>> {
  auto operator()(const Source &source) const {
    return unicode::as<std::string>(unicode::views::as_u16(source) | unicode::transforms::to_u8);
  }
};

template<class Source>
struct to_u8_impl<
    Source, std::enable_if_t<is_string_convertible_v<Source, char16_t>>> {
  auto operator()(const Source &source) const {
    return unicode::as<std::string>(unicode::views::as_u16(source) | unicode::transforms::to_u8);
  }
};

template<class Source>
struct to_u8_impl<
    Source, std::enable_if_t<is_string_convertible_v<Source, char32_t>>> {
  auto operator()(const Source &source) const {
    return unicode::as<std::string>(source | unicode::transforms::to_u8);
  }
};

template<typename Source>
inline auto to_u8(const Source &source) {
  to_u8_impl<Source> to_u8;
  return to_u8(source);
}
}  // namespace details
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_UNICODE_DETAILS_TO_U8_HPP
