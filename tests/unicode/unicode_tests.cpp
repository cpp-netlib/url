// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <string>
#include <skyr/unicode/ranges/views/u8_view.hpp>
#include <skyr/unicode/ranges/views/u16_view.hpp>
#include <skyr/unicode/ranges/views/u32_view.hpp>
#include <skyr/unicode/ranges/transforms/byte_transform.hpp>
#include <skyr/unicode/ranges/transforms/u16_transform.hpp>
#include <skyr/unicode/ranges/transforms/u32_transform.hpp>


TEST_CASE("unicode_tests", "[unicode]") {
  SECTION("utf32_to_bytes_poo_emoji_test") {
    auto input = std::u32string(U"\x1F4A9");
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::view::as_u32 | skyr::unicode::transform::to_bytes);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_utf32_poo_emoji_test") {
    auto input = std::string("\xf0\x9f\x92\xa9");
    auto utf32 = skyr::unicode::as<std::u32string>(
        input | skyr::unicode::view::as_u8 | skyr::unicode::transform::to_u32);
    REQUIRE(utf32);
    CHECK(U"\x1F4A9" == utf32.value());
  }

  SECTION("utf16_to_bytes_poo_emoji_test") {
    auto input = std::u16string(u"\xd83d\xdca9");
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::view::as_u16 | skyr::unicode::transform::to_bytes);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_utf16_poo_emoji_test") {
    auto input = std::string("\xf0\x9f\x92\xa9");
    auto utf16 = skyr::unicode::as<std::u16string>(
        input | skyr::unicode::view::as_u8 | skyr::unicode::transform::to_u16);
    REQUIRE(utf16);
    CHECK(u"\xd83d\xdca9" == utf16.value());
  }

  SECTION("wstring_to_bytes_poo_emoji_test") {
    auto input = std::wstring(L"\xd83d\xdca9");
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::view::as_u16 | skyr::unicode::transform::to_bytes);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_wstring_poo_emoji_test") {
    auto input = std::string("\xf0\x9f\x92\xa9");
    auto utf16 = skyr::unicode::as<std::wstring>(
        input | skyr::unicode::view::as_u8 | skyr::unicode::transform::to_u16);
    REQUIRE(utf16);
    CHECK(L"\xd83d\xdca9" == utf16.value());
  }

  SECTION("utf32_rainbow_flag_test") {
    auto input = std::u32string(U"\x1F3F3\xFE0F\x200D\x1F308");
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::view::as_u32 | skyr::unicode::transform::to_bytes);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }
}
