// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_all.hpp>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <skyr/v2/percent_encoding/percent_encoded_char.hpp>

TEST_CASE("encode fragment", "[percent_encoding]") {
  auto c = GENERATE(
      ' ', '\"', '<', '>', '`');

  SECTION("encode_fragment_set") {
    auto encoded = skyr::percent_encoding::percent_encode_byte(
        std::byte(c), skyr::percent_encoding::encode_set::fragment);
    CHECK(encoded.is_encoded());
  }
}

TEST_CASE("encode path", "[percent_encoding]") {
  auto c = GENERATE(
      ' ', '\"', '<', '>', '`', '#', '?', '{', '}');

  SECTION("encode_path_set") {
    auto encoded = skyr::percent_encoding::percent_encode_byte(
        std::byte(c), skyr::percent_encoding::encode_set::path);
    CHECK(encoded.is_encoded());
  }
}

TEST_CASE("encode userinfo", "[percent_encoding]") {
  auto c = GENERATE(
      ' ', '\"', '<', '>', '`', '#', '?', '{', '}', '/',
      ':', ';', '=', '@', '[', '\\', ']', '^', '|');

  SECTION("encode_userinfo_set") {
    auto encoded = skyr::percent_encoding::percent_encode_byte(
        std::byte(c), skyr::percent_encoding::encode_set::userinfo);
    CHECK(encoded.is_encoded());
  }
}

TEST_CASE("encode_tests", "[percent_encoding]") {
  using namespace std::string_literals;

  SECTION("encode_codepoints_before_0x20_set") {
    for (auto i = 0u; i < 0x20u; ++i) {
      auto encoded = skyr::percent_encoding::percent_encode_byte(
          std::byte(i), skyr::percent_encoding::encode_set::c0_control);
      CHECK(fmt::format("%{:02X}", i) == encoded.to_string());
    }
  }

  SECTION("encode_codepoints_before_0x7e_set") {
    for (auto i = 0x7fu; i <= 0xffu; ++i) {
      auto encoded = skyr::percent_encoding::percent_encode_byte(
          std::byte(i), skyr::percent_encoding::encode_set::c0_control);
      CHECK(fmt::format("%{:02X}", i) == encoded.to_string());
    }
  }

  SECTION("encode_0x25") {
    auto encoded = skyr::percent_encoding::percent_encode_byte(
        std::byte(0x25), skyr::percent_encoding::encode_set::any);
    CHECK("%25" == encoded.to_string());
  }

  SECTION("encode_0x2b") {
    auto encoded = skyr::percent_encoding::percent_encode_byte(
        std::byte(0x2b), skyr::percent_encoding::encode_set::any);
    CHECK("%2B" == encoded.to_string());
  }
}
