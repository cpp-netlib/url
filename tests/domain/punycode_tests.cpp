// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <string>
#include "../../src/v1/domain/punycode.hpp"


TEST_CASE("encode_test", "[punycode]") {
  using namespace std::string_literals;

  auto domain = GENERATE(
      std::make_pair("你好你好"s, "6qqa088eba"s),
      std::make_pair("你"s, "6qq"s),
      std::make_pair("好"s, "5us"s),
      std::make_pair("你好"s, "6qq79v"s),
      std::make_pair("你好你"s, "6qqa088e"s),
      std::make_pair("點看"s, "c1yn36f"s),
      std::make_pair("faß"s, "fa-hia"s),
      std::make_pair("☃"s, "n3h"s),
      std::make_pair("bücher"s, "bcher-kva"s),
      std::make_pair("ü"s, "tda"s),
      std::make_pair("⌘"s, "bih"s),
      std::make_pair("ñ"s, "ida"s),
      std::make_pair("☃"s, "n3h"s),
      std::make_pair("उदाहरण"s, "p1b6ci4b4b3a"s),
      std::make_pair("परीक्षा"s, "11b5bs3a9aj6g"s),
      std::make_pair("glyn"s, "glyn-"s)
  );

  SECTION("encode_set") {
    const auto &[input, expected] = domain;
    auto encoded = skyr::punycode_encode(input);
    REQUIRE(encoded);
    CHECK(expected == encoded.value());
  }

  SECTION("decode_set") {
    const auto &[expected, input] = domain;
    auto decoded = skyr::punycode_decode(input);
    REQUIRE(decoded);
    CHECK(expected == decoded.value());
  }
}
