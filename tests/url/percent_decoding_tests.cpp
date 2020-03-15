// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/url/percent_encoding/percent_decode_range.hpp>
#include <skyr/unicode/ranges/views/u8_view.hpp>
#include <skyr/unicode/ranges/transforms/u32_transform.hpp>


TEST_CASE("percent_decode", "[percent_decode]") {
  SECTION("decode_codepoints_set") {
    for (auto i = 0x00; i < 0xff; ++i) {
      char buffer[8];
      std::snprintf(buffer, sizeof(buffer), "%02X", i);
      auto input = std::string("%") + buffer;
      auto decoded = skyr::percent_encoding::as<std::string>(
          input | skyr::percent_encoding::view::decode);
      REQUIRE(decoded);
      CHECK(std::string{static_cast<char>(i)} == decoded.value());
    }
  }

  SECTION("u8_test") {
    auto input = std::string("%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88");
    auto decoded = skyr::percent_encoding::as<std::string>(
        input | skyr::percent_encoding::view::decode);
    REQUIRE(decoded);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == decoded);
  }

  SECTION("empty string") {
    auto input = std::string();
    auto decoded = skyr::percent_encoding::as<std::string>(
        input | skyr::percent_encoding::view::decode);
    CHECK(decoded);
  }
}
