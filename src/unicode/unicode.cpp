// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <skyr/unicode/unicode.hpp>
#include <skyr/unicode/range/transforms/u16_transform.hpp>
#include <skyr/unicode/range/transforms/u32_transform.hpp>
#include <skyr/unicode/range/transforms/byte_transform.hpp>


namespace skyr::unicode {
tl::expected<std::wstring, std::error_code> wstring_from_bytes(
    std::string_view bytes) {
  return as<std::wstring>(bytes | view::as_u8 | transform::to_u16);
}

tl::expected<std::string, std::error_code> wstring_to_bytes(
    std::wstring_view input) {
  std::string result;
  auto expected = copy_u16u8(
      begin(input), end(input), std::back_inserter(result));
  if (!expected) {
    return tl::make_unexpected(std::error_code(expected.error()));
  }
  return result;
}

tl::expected<std::u16string, std::error_code> utf16_from_bytes(
    std::string_view bytes) {
  return as<std::u16string>(bytes | view::as_u8 | transform::to_u16);
}

tl::expected<std::string, std::error_code> utf16_to_bytes(
    std::u16string_view input) {
  std::string result;
  auto expected = copy_u16u8(
      begin(input), end(input), std::back_inserter(result));
  if (!expected) {
    return tl::make_unexpected(std::error_code(expected.error()));
  }
  return result;
}

tl::expected<std::u32string, std::error_code> utf32_from_bytes(
    std::string_view bytes) {
  return as<std::u32string>(bytes | view::as_u8 | transform::to_u32);
}

tl::expected<std::string, std::error_code> utf32_to_bytes(
    std::u32string_view input) {
  return as<std::string>(input | transform::to_bytes);
}
}  // namespace skyr::unicode
