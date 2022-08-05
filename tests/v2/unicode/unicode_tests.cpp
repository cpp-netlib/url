// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_all.hpp>
#include <skyr/v2/unicode/ranges/transforms/u16_transform.hpp>
#include <skyr/v2/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/v2/unicode/ranges/transforms/u8_transform.hpp>
#include <skyr/v2/unicode/ranges/views/u16_view.hpp>
#include <skyr/v2/unicode/ranges/views/u8_view.hpp>
#include <string>

TEST_CASE("unicode_tests", "[unicode]") {
  using namespace std::string_literals;
  using namespace std::string_view_literals;

  SECTION("utf32_to_bytes_poo_emoji_test") {
    const auto input = U"\x1F4A9"s;
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_utf32_poo_emoji_test") {
    const auto input = "\xf0\x9f\x92\xa9"s;
    auto utf32 = skyr::unicode::as<std::u32string>(
        skyr::unicode::views::as_u8(input) | skyr::unicode::transforms::to_u32);
    REQUIRE(utf32);
    CHECK(U"\x1F4A9" == utf32.value());
  }

  SECTION("utf16_to_bytes_poo_emoji_test") {
    const auto input = u"\xd83d\xdca9"s;
    auto bytes = skyr::unicode::as<std::string>(
        skyr::unicode::views::as_u16(input) | skyr::unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_utf16_poo_emoji_test") {
    const auto input = "\xf0\x9f\x92\xa9"s;
    auto utf16 = skyr::unicode::as<std::u16string>(
        skyr::unicode::views::as_u8(input) | skyr::unicode::transforms::to_u16);
    REQUIRE(utf16);
    CHECK(u"\xd83d\xdca9" == utf16.value());
  }

  SECTION("wstring_to_bytes_poo_emoji_test") {
    const auto input = L"\xd83d\xdca9"s;
    auto bytes = skyr::unicode::as<std::string>(
        skyr::unicode::views::as_u16(input) | skyr::unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_wstring_poo_emoji_test") {
    const auto input = "\xf0\x9f\x92\xa9"s;
    auto utf16 = skyr::unicode::as<std::wstring>(
        skyr::unicode::views::as_u8(input) | skyr::unicode::transforms::to_u16);
    REQUIRE(utf16);
    CHECK(L"\xd83d\xdca9" == utf16.value());
  }

  SECTION("utf32_rainbow_flag_test") {
    const auto input = U"\x1F3F3\xFE0F\x200D\x1F308"s;
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }

  SECTION("u32_to_u8_sv_1") {
    const auto input =
        U"\x0000\x0001\t\n\r\x001f !\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\x007f\x0080\x0081\x00c9\x00e9"sv;
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK(
        "\x00\x01\x09\x0a\x0d\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28"
        "\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x39\x3a\x3b\x3c\x3d\x3e\x3f"
        "\x40\x41\x5a\x5b\x5c\x5d\x5e\x5f\x60\x61\x7a\x7b\x7c\x7d\x7e"
        "\x7f\xc2\x80\xc2\x81\xc3\x89\xc3\xa9"sv == bytes.value());
  }

  SECTION("u32_to_u8_sv_2") {
    const auto input = U"\x0080\x0081\x00c9\x00e9"sv;
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK("\xc2\x80\xc2\x81\xc3\x89\xc3\xa9"sv == bytes.value());
  }

  SECTION("u16_to_u8_sv_1") {
    const auto input =
        u"\x0000\x0001\t\n\r\x001f !\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\x007f\x0080\x0081\x00c9\x00e9"sv;
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK(
        "\x00\x01\x09\x0a\x0d\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28"
        "\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x39\x3a\x3b\x3c\x3d\x3e\x3f"
        "\x40\x41\x5a\x5b\x5c\x5d\x5e\x5f\x60\x61\x7a\x7b\x7c\x7d\x7e"
        "\x7f\xc2\x80\xc2\x81\xc3\x89\xc3\xa9"sv == bytes.value());
  }

  SECTION("u16_to_u8_sv_2") {
    const auto input = u"\x0080\x0081\x00c9\x00e9"sv;
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK("\xc2\x80\xc2\x81\xc3\x89\xc3\xa9"sv == bytes.value());
  }
}
