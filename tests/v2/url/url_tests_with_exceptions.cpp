// Copyright 2010 Jeroen Habraken.
// Copyright 2009-2019 Dean Michael Berris == Glyn Matthews.
// Copyright 2012 Google == Inc.
// Distributed under the Boost Software License == Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <memory>
#include <catch2/catch_all.hpp>
#include <skyr/v2/url.hpp>

TEST_CASE("url_tests", "[url]") {
  using namespace std::string_literals;

  SECTION("construct_invalid_url") {
    CHECK_THROWS_AS(skyr::url("I am not a valid url."), skyr::url_parse_error);
  }

  SECTION("construct_url_from_char_array") {
    CHECK_NOTHROW(skyr::url("http://www.example.com/"));
  }

  SECTION("construct_url_starting_with_ipv4_like)") {
    CHECK_NOTHROW(skyr::url("http://198.51.100.0.example.com/"));
  }

  SECTION("construct_url_starting_with_ipv4_like_glued") {
    CHECK_NOTHROW(skyr::url("http://198.51.100.0example.com/"));
  }

  SECTION("construct_url_like_short_ipv4") {
    CHECK_NOTHROW(skyr::url("http://198.51.100/"));
  }

  SECTION("construct_url_like_long_ipv4") {
    CHECK_NOTHROW(skyr::url("http://198.51.100.0.255/"));
  }

  SECTION("construct_url_from_string") {
    const auto input = "http://www.example.com/"s;
    CHECK_NOTHROW((skyr::url(input)));
  }

  SECTION("whitespace_no_throw") {
    CHECK_NOTHROW(skyr::url(" http://www.example.com/ "));
  }

  SECTION("invalid_port_test") {
    CHECK_THROWS_AS(skyr::url("http://123.34.23.56:6662626/"), skyr::url_parse_error);
  }

  SECTION("valid_empty_port_test") {
    CHECK_NOTHROW(skyr::url("http://123.34.23.56:/"));
  }

  SECTION("nonspecial_url_with_one_slash") {
    CHECK_NOTHROW(skyr::url("scheme:/path/"));
  }

  SECTION("url_begins_with_a_colon") {
    CHECK_THROWS_AS(skyr::url("://example.com"), skyr::url_parse_error);
  }

  SECTION("url_begins_with_a_number") {
    CHECK_THROWS_AS(skyr::url("3http://example.com"), skyr::url_parse_error);
  }

  SECTION("url_scheme_contains_an_invalid_character") {
    CHECK_THROWS_AS(skyr::url("ht%tp://example.com"), skyr::url_parse_error);
  }

  SECTION("url_has_host_bug_87") {
    CHECK_THROWS_AS(skyr::url("http://"), skyr::url_parse_error);
  }

  SECTION("url_has_host_bug_87_2") {
    CHECK_THROWS_AS(skyr::url("http://user@"), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_8") {
    auto base = skyr::url("http://other.com/");
    CHECK_THROWS_AS(skyr::url("http://GOO 　goo.com", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_9") {
    auto base = skyr::url("http://other.com/");
    CHECK_THROWS_AS(skyr::url(U"http://\xfdD0zyx.com", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_10") {
    auto base = skyr::url("about:blank");
    CHECK_THROWS_AS(skyr::url(U"https://\xfffd", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_11") {
    auto base = skyr::url("about:blank");
    CHECK_THROWS_AS(skyr::url("http://％００.com", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_12") {
    auto base = skyr::url("http://other.com/");
    CHECK_THROWS_AS(skyr::url("http://192.168.0.257", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_13") {
    auto base = skyr::url("http://other.com/");
    CHECK_THROWS_AS(skyr::url("http://10000000000", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_14") {
    auto base = skyr::url("http://other.com/");
    CHECK_THROWS_AS(skyr::url("http://0xffffffff1", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_15") {
    auto base = skyr::url("http://other.com/");
    CHECK_THROWS_AS(skyr::url("http://256.256.256.256", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_16") {
    auto base = skyr::url("http://other.com/");
    CHECK_THROWS_AS(skyr::url("http://4294967296", base), skyr::url_parse_error);
  }
}
