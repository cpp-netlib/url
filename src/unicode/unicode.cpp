// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <skyr/unicode/unicode.hpp>
#include "unicode/unicode_impl.hpp"

namespace skyr::unicode {
namespace {
class unicode_error_category : public std::error_category {
 public:
  [[nodiscard]] const char *name() const noexcept override;
  [[nodiscard]] std::string message(int error) const noexcept override;
};

const char *unicode_error_category::name() const noexcept {
  return "unicode";
}

std::string unicode_error_category::message(int error) const noexcept {
  switch (static_cast<unicode_errc>(error)) {
    case unicode_errc::overflow:return "Overflow";
    case unicode_errc::invalid_lead:return "Invalid lead";
    case unicode_errc::illegal_byte_sequence:return "Illegal byte sequence";
    case unicode_errc::invalid_code_point:return "Invalid code point";
    default:return "(Unknown error)";
  }
}

const unicode_error_category category{};
}  // namespace

std::error_code make_error_code(unicode_errc error) {
  return std::error_code(static_cast<int>(error), category);
}

tl::expected<std::wstring, std::error_code> wstring_from_bytes(
    std::string_view input) {
  std::wstring result;
  auto expected = utf8to16(
      begin(input), end(input), std::back_inserter(result));
  if (!expected) {
    return tl::make_unexpected(std::error_code(expected.error()));
  }
  return result;
}

tl::expected<std::string, std::error_code> wstring_to_bytes(
    std::wstring_view input) {
  std::string result;
  auto expected = utf16to8(
      begin(input), end(input), std::back_inserter(result));
  if (!expected) {
    return tl::make_unexpected(std::error_code(expected.error()));
  }
  return result;
}

tl::expected<std::u16string, std::error_code> utf16_from_bytes(
    std::string_view bytes) {
  std::u16string result;
  auto expected = utf8to16(
      begin(bytes), end(bytes), std::back_inserter(result));
  if (!expected) {
    return tl::make_unexpected(std::error_code(expected.error()));
  }
  return result;
}

tl::expected<std::string, std::error_code> utf16_to_bytes(
    std::u16string_view input) {
  std::string result;
  auto expected = utf16to8(
      begin(input), end(input), std::back_inserter(result));
  if (!expected) {
    return tl::make_unexpected(std::error_code(expected.error()));
  }
  return result;
}

tl::expected<std::u32string, std::error_code> utf32_from_bytes(
    std::string_view bytes) {
  std::u32string result;
  auto expected = utf8to32(
      begin(bytes), end(bytes), std::back_inserter(result));
  if (!expected) {
    return tl::make_unexpected(std::error_code(expected.error()));
  }
  return result;
}

tl::expected<std::string, std::error_code> utf32_to_bytes(
    std::u32string_view input) {
  std::string result;
  auto expected = utf32to8(
      begin(input), end(input), std::back_inserter(result));
  if (!expected) {
    return tl::make_unexpected(std::error_code(expected.error()));
  }
  return result;
}
}  // namespace skyr::unicode
