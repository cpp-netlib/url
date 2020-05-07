// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <skyr/v1/percent_encoding/percent_decode_range.hpp>


TEST_CASE("percent_decode", "[percent_decode]") {
  using namespace std::string_literals;

  SECTION("decode_codepoints_set") {
    for (auto i = 0x00; i < 0xff; ++i) {
      auto decoded = skyr::percent_encoding::as<std::string>(
          fmt::format("%{:02X}", i) | skyr::percent_encoding::view::decode);
      REQUIRE(decoded);
      CHECK(std::string{static_cast<char>(i)} == decoded.value());
    }
  }

  SECTION("u8_test") {
    auto decoded = skyr::percent_encoding::as<std::string>(
        "%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88"s | skyr::percent_encoding::view::decode);
    REQUIRE(decoded);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == decoded);
  }

  SECTION("empty string") {
    auto decoded = skyr::percent_encoding::as<std::string>(
        ""s | skyr::percent_encoding::view::decode);
    CHECK(decoded);
  }
}
