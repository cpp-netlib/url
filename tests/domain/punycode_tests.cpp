// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <skyr/v1/unicode/ranges/views/u8_view.hpp>
#include <skyr/v1/unicode/ranges/transforms/u32_transform.hpp>
#include "../../src/v1/domain/punycode.hpp"


TEST_CASE("encode_test", "[punycode]") {
  using namespace std::string_literals;

  auto domain = GENERATE(
      std::make_pair(U"你好你好"s, "6qqa088eba"s),
      std::make_pair(U"你"s, "6qq"s),
      std::make_pair(U"好"s, "5us"s),
      std::make_pair(U"你好"s, "6qq79v"s),
      std::make_pair(U"你好你"s, "6qqa088e"s),
      std::make_pair(U"點看"s, "c1yn36f"s),
      std::make_pair(U"faß"s, "fa-hia"s),
      std::make_pair(U"☃"s, "n3h"s),
      std::make_pair(U"bücher"s, "bcher-kva"s),
      std::make_pair(U"ü"s, "tda"s),
      std::make_pair(U"⌘"s, "bih"s),
      std::make_pair(U"ñ"s, "ida"s),
      std::make_pair(U"☃"s, "n3h"s),
      std::make_pair(U"उदाहरण"s, "p1b6ci4b4b3a"s),
      std::make_pair(U"परीक्षा"s, "11b5bs3a9aj6g"s),
      std::make_pair(U"glyn"s, "glyn-"s)
  );

  SECTION("encode_set") {
    const auto &[input, expected] = domain;
    auto encoded = std::string{};
    auto result = skyr::punycode_encode(input, &encoded);
    REQUIRE(result);
    CHECK(expected == encoded);
  }

  SECTION("decode_set") {
    const auto &[expected, input] = domain;
    auto decoded = std::u32string{};
    auto result = skyr::punycode_decode(std::string_view(input), &decoded);
    REQUIRE(result);
    CHECK(expected == decoded);
  }
}

TEST_CASE("special_strings") {
  using namespace std::string_view_literals;

  SECTION("U+FFFD") {
    auto decoded = std::u32string{};
    auto result = skyr::punycode_decode("zn7c"sv, &decoded);
    REQUIRE(result);
    CHECK(U"\xfffd" == decoded);
  }
}
