// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <skyr/percent_encoding/percent_encode_range.hpp>

TEST_CASE("encode fragment", "[percent_encoding]") {
  auto c = GENERATE(
      ' ', '\"', '<', '>', '`');

  SECTION("encode_fragment_set") {
    auto encoded = skyr::percent_encoding::percent_encode_byte(c, skyr::percent_encoding::encode_set::fragment);
    CHECK(encoded.is_encoded());
  }
}

TEST_CASE("encode path", "[percent_encoding]") {
  auto c = GENERATE(
      ' ', '\"', '<', '>', '`', '#', '?', '{', '}');

  SECTION("encode_path_set") {
    auto encoded = skyr::percent_encoding::percent_encode_byte(c, skyr::percent_encoding::encode_set::path);
    CHECK(encoded.is_encoded());
  }
}

TEST_CASE("encode userinfo", "[percent_encoding]") {
  auto c = GENERATE(
      ' ', '\"', '<', '>', '`', '#', '?', '{', '}', '/',
      ':', ';', '=', '@', '[', '\\', ']', '^', '|');

  SECTION("encode_userinfo_set") {
    auto encoded = skyr::percent_encoding::percent_encode_byte(c, skyr::percent_encoding::encode_set::userinfo);
    CHECK(encoded.is_encoded());
  }
}

TEST_CASE("encode_tests", "[percent_encoding]") {
  using namespace std::string_literals;

  SECTION("encode_codepoints_before_0x20_set") {
    for (auto i = 0u; i < 0x20u; ++i) {
      auto encoded = skyr::percent_encoding::percent_encode_byte(i, skyr::percent_encoding::encode_set::c0_control);
      CHECK(fmt::format("%{:02X}", i) == encoded.to_string());
    }
  }

  SECTION("encode_codepoints_before_0x7e_set") {
    for (auto i = 0x7fu; i <= 0xffu; ++i) {
      auto encoded = skyr::percent_encoding::percent_encode_byte(i, skyr::percent_encoding::encode_set::c0_control);
      CHECK(fmt::format("%{:02X}", i) == encoded.to_string());
    }
  }

  SECTION("u8_test_1") {
    const auto input = "\xf0\x9f\x92\xa9"s;
    auto encoded = skyr::percent_encoding::as<std::string>(input | skyr::percent_encoding::view::encode);
    CHECK("%F0%9F%92%A9" == encoded);
  }

  SECTION("u8_test_2") {
    const auto input = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"s;
    auto encoded = skyr::percent_encoding::as<std::string>(input | skyr::percent_encoding::view::encode);
    CHECK("%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == encoded);
  }
}
