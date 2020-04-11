// Copyright (c) Glyn Matthews 2012-2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_DETAILS_TRANSLATE_INC
#define SKYR_URL_DETAILS_TRANSLATE_INC

#include <string>
#include <skyr/traits/string_traits.hpp>
#include <skyr/unicode/ranges/views/u16_view.hpp>
#include <skyr/unicode/ranges/transforms/byte_transform.hpp>

namespace skyr {
inline namespace v1 {
namespace details {
template<class Source, class Enable = void>
struct to_bytes_impl;

template<class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, char>::value>::type> {
  tl::expected<std::string, std::error_code> operator()(const Source &source) const {
    return std::string(source);
  }
};

template<class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, wchar_t>::value>::type> {
  tl::expected<std::string, std::error_code> operator()(const Source &source) const {
    return unicode::as<std::string>(unicode::view::as_u16(source) | unicode::transform::to_bytes);
  }
};

template<class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, char16_t>::value>::type> {
  tl::expected<std::string, std::error_code> operator()(const Source &source) const {
    return unicode::as<std::string>(unicode::view::as_u16(source) | unicode::transform::to_bytes);
  }
};

template<class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, char32_t>::value>::type> {
  tl::expected<std::string, std::error_code> operator()(const Source &source) const {
    return unicode::as<std::string>(source | unicode::transform::to_bytes);
  }
};

template<typename Source>
inline tl::expected<std::string, std::error_code> to_bytes(const Source &source) {
  to_bytes_impl<Source> to_bytes;
  return to_bytes(source);
}
}  // namespace details
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_URL_DETAILS_TRANSLATE_INC
