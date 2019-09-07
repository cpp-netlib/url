// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <string>
#include "skyr/unicode/unicode.hpp"
#include "../src/unicode/unicode_impl.hpp"


TEST_CASE("unicode_tests", "[unicode]") {
  SECTION("utf32_to_bytes_poo_emoji_test") {
    auto input = std::u32string(U"\x1F4A9");
    auto bytes = skyr::unicode::utf32_to_bytes(input);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x92\xa9" == bytes.value());
  }

  SECTION("bytes_to_utf32_poo_emoji_test") {
    auto input = std::string("\xf0\x9f\x92\xa9");
    auto utf32 = skyr::unicode::utf32_from_bytes(input);
    REQUIRE(utf32);
    CHECK(U"\x1F4A9" == utf32.value());
  }

  SECTION("utf32_rainbow_flag_test") {
    auto input = std::u32string(U"\x1F3F3\xFE0F\x200D\x1F308");
    auto bytes = skyr::unicode::utf32_to_bytes(input);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }

  SECTION("count_utf8_chars_01") {
    auto bytes = std::string("\xf0\x9f\x92\xa9");
    auto first = begin(bytes), last = end(bytes);
    auto count = skyr::unicode::count(first, last);
    REQUIRE(count);
    CHECK(1 == count.value());
  }

  SECTION("count_utf8_chars_02") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto first = begin(bytes), last = end(bytes);
    auto count = skyr::unicode::count(first, last);
    REQUIRE(count);
    CHECK(4 == count.value());
  }

  SECTION("advance_utf8_chars") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto first = begin(bytes), last = end(bytes);
    skyr::unicode::advance(first, 2, last);
    auto count = skyr::unicode::count(first, last);
    REQUIRE(count);
    CHECK(2 == count.value());
  }
}
