// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <skyr/v1/unicode/ranges/transforms/u16_transform.hpp>
#include <skyr/v1/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/v1/unicode/ranges/transforms/u8_transform.hpp>
#include <skyr/v1/unicode/ranges/views/u16_view.hpp>
#include <skyr/v1/unicode/ranges/views/u8_view.hpp>
#include <string>

TEST_CASE("unicode_tests", "[unicode]") {
  using namespace std::string_literals;

  SECTION("utf32_to_bytes_poo_emoji_test") {
    const auto input = U"\x1F4A9"s;
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::transform::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_utf32_poo_emoji_test") {
    const auto input = "\xf0\x9f\x92\xa9"s;
    auto utf32 = skyr::unicode::as<std::u32string>(
        skyr::unicode::view::as_u8(input) | skyr::unicode::transform::to_u32);
    REQUIRE(utf32);
    CHECK(U"\x1F4A9" == utf32.value());
  }

  SECTION("utf16_to_bytes_poo_emoji_test") {
    const auto input = u"\xd83d\xdca9"s;
    auto bytes = skyr::unicode::as<std::string>(
        skyr::unicode::view::as_u16(input) | skyr::unicode::transform::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_utf16_poo_emoji_test") {
    const auto input = "\xf0\x9f\x92\xa9"s;
    auto utf16 = skyr::unicode::as<std::u16string>(
        skyr::unicode::view::as_u8(input) | skyr::unicode::transform::to_u16);
    REQUIRE(utf16);
    CHECK(u"\xd83d\xdca9" == utf16.value());
  }

  SECTION("wstring_to_bytes_poo_emoji_test") {
    const auto input = L"\xd83d\xdca9"s;
    auto bytes = skyr::unicode::as<std::string>(
        skyr::unicode::view::as_u16(input) | skyr::unicode::transform::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_wstring_poo_emoji_test") {
    const auto input = "\xf0\x9f\x92\xa9"s;
    auto utf16 = skyr::unicode::as<std::wstring>(
        skyr::unicode::view::as_u8(input) | skyr::unicode::transform::to_u16);
    REQUIRE(utf16);
    CHECK(L"\xd83d\xdca9" == utf16.value());
  }

  SECTION("utf32_rainbow_flag_test") {
    const auto input = U"\x1F3F3\xFE0F\x200D\x1F308"s;
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::transform::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }
}
