// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <string_view>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <range/v3/empty.hpp>
#include <skyr/unicode/code_point.hpp>


TEST_CASE("u8 code point tests") {
  using std::begin;
  using std::end;

  SECTION("code point 01") {
    auto bytes = std::string("\xf0\x9f\x92\xa9");
    auto cp = skyr::unicode::u8_code_point(bytes);
    REQUIRE(cp);
    CHECK(std::string("\xf0\x9f\x92\xa9") == std::string(begin(cp.value()), end(cp.value())));
    CHECK(U'\x1f4a9' == u32_value(cp));
    CHECK(u16_value(cp).value().is_surrogate_pair());
    CHECK(u'\xd83d' == u16_value(cp).value().lead_value());
    CHECK(u'\xdca9' == u16_value(cp).value().trail_value());
  }

  SECTION("code point fail") {
    auto bytes = std::string("\x9f\x92\xa9");
    auto cp = skyr::unicode::u8_code_point(bytes);
    REQUIRE(!cp);
  }
}

TEST_CASE("u16 code point tests") {
  using std::begin;
  using std::end;

  SECTION("code point 01") {
    auto lead = u'\xD83C', trail = u'\xDFF3';
    auto cp = skyr::unicode::u16_code_point(lead, trail);
    CHECK(U'\x1F3F3' == u32_value(cp));
    CHECK(cp.is_surrogate_pair());
    CHECK(lead == cp.lead_value());
    CHECK(trail == cp.trail_value());
  }
}
