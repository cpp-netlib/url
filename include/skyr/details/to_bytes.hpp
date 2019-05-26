// Copyright (c) Glyn Matthews 2012-2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_DETAILS_TRANSLATE_INC
#define SKYR_URL_DETAILS_TRANSLATE_INC

#include <string>
#include <skyr/string_traits.hpp>
#include <skyr/unicode.hpp>

namespace skyr {
namespace details {
template <class Source, class Enable = void>
struct to_bytes_impl;

template <class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, char>::value>::type> {
  expected<std::string, std::error_code> operator()(const Source &source) const {
    return source;
  }
};

template <class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, wchar_t>::value>::type> {
  expected<std::string, std::error_code> operator()(const Source &source) const {
    return unicode::wstring_to_bytes(source);
  }
};

template <class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, char16_t>::value>::type> {
  expected<std::string, std::error_code> operator()(const Source &source) const {
    return unicode::utf16_to_bytes(source);
  }
};

template <class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, char32_t>::value>::type> {
  expected<std::string, std::error_code> operator()(const Source &source) const {
    return unicode::utf32_to_bytes(source);
  }
};

template <typename Source>
inline expected<std::string, std::error_code> to_bytes(const Source &source) {
  to_bytes_impl<Source> to_bytes;
  return to_bytes(source);
}
}  // namespace details
}  // namespace skyr

#endif  // SKYR_URL_DETAILS_TRANSLATE_INC
