// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <catch2/catch_all.hpp>
#include <skyr/v2/domain/punycode.hpp>


TEST_CASE("encode_test", "[punycode]") {
  using namespace std::string_literals;

  auto domain = GENERATE(
      std::make_pair(U"\x4F60\x597D\x4F60\x597D"s, "6qqa088eba"s),
      std::make_pair(U"\x4F60"s, "6qq"s),
      std::make_pair(U"\x597D"s, "5us"s),
      std::make_pair(U"\x4F60\x597D"s, "6qq79v"s),
      std::make_pair(U"\x4F60\x597D\x4F60"s, "6qqa088e"s),
      std::make_pair(U"\x9EDE\x770B"s, "c1yn36f"s),
      std::make_pair(U"fa\x00DF"s, "fa-hia"s),
      std::make_pair(U"\x2603"s, "n3h"s),
      std::make_pair(U"b\x00FC\x0063her"s, "bcher-kva"s),
      std::make_pair(U"\x00FC"s, "tda"s),
      std::make_pair(U"\x2318"s, "bih"s),
      std::make_pair(U"\x00F1"s, "ida"s),
      std::make_pair(U"\x2603"s, "n3h"s),
      std::make_pair(U"\x0909\x0926\x093E\x0939\x0930\x0923"s, "p1b6ci4b4b3a"s),
      std::make_pair(U"\x092A\x0930\x0940\x0915\x094D\x0937\x093E"s, "11b5bs3a9aj6g"s),
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
