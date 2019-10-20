// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <string>
#include "skyr/unicode/domain.hpp"


TEST_CASE("encode_test", "[punycode]") {
  using param = std::pair<std::string, std::string>;

  auto domain = GENERATE(
      param{u8"你好你好", "xn--6qqa088eba"},
      param{u8"你", "xn--6qq"},
      param{u8"好", "xn--5us"},
      param{u8"你好", "xn--6qq79v"},
      param{u8"你好你", "xn--6qqa088e"},
      param{u8"點看", "xn--c1yn36f"},
      param{u8"faß", "xn--fa-hia"},
      param{u8"☃", "xn--n3h"},
      param{u8"bücher", "xn--bcher-kva"},
      param{u8"ü", "xn--tda"},
      param{u8"⌘", "xn--bih"},
      param{u8"ñ", "xn--ida"},
      param{u8"☃", "xn--n3h"},
      param{u8"उदाहरण", "xn--p1b6ci4b4b3a"},
      param{u8"परीक्षा", "xn--11b5bs3a9aj6g"},
      param{u8"glyn", "xn--glyn-"}
  );

  SECTION("encode_set") {
    auto [input, expected] = domain;
    auto encoded = skyr::unicode::punycode_encode(input);
    REQUIRE(encoded);
    CHECK(expected == encoded.value());
  }

  SECTION("decode_set") {
    auto [expected, input] = domain;
    auto decoded = skyr::unicode::punycode_decode(input);
    REQUIRE(decoded);
    CHECK(expected == decoded.value());
  }
}
