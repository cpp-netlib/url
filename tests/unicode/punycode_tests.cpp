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
      param{"你好你好", "xn--6qqa088eba"},
      param{"你", "xn--6qq"},
      param{"好", "xn--5us"},
      param{"你好", "xn--6qq79v"},
      param{"你好你", "xn--6qqa088e"},
      param{"點看", "xn--c1yn36f"},
      param{"faß", "xn--fa-hia"},
      param{"☃", "xn--n3h"},
      param{"bücher", "xn--bcher-kva"},
      param{"ü", "xn--tda"},
      param{"⌘", "xn--bih"},
      param{"ñ", "xn--ida"},
      param{"☃", "xn--n3h"},
      param{"उदाहरण", "xn--p1b6ci4b4b3a"},
      param{"परीक्षा", "xn--11b5bs3a9aj6g"},
      param{"glyn", "xn--glyn-"}
  );

  SECTION("encode_set") {
    const auto &[input, expected] = domain;
    auto encoded = skyr::unicode::punycode_encode(input);
    REQUIRE(encoded);
    CHECK(expected == encoded.value());
  }

  SECTION("decode_set") {
    const auto &[expected, input] = domain;
    auto decoded = skyr::unicode::punycode_decode(input);
    REQUIRE(decoded);
    CHECK(expected == decoded.value());
  }
}
