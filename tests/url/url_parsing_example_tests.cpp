// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/url/url_parse.hpp>
#include <skyr/url/url_serialize.hpp>

/// https://url.spec.whatwg.org/#example-url-parsing

TEST_CASE("url_parsing_example_tests", "[parse]") {
  SECTION("url_path_1") {
    auto instance = skyr::parse("https:example.org");
    REQUIRE(instance);
    CHECK(1 == instance.value().path.size());
  }

  SECTION("url_path_2") {
    auto instance = skyr::parse("https://////example.com///");
    REQUIRE(instance);
    CHECK(3 == instance.value().path.size());
  }

  SECTION("url_serialize_1") {
    auto input = std::string("https:example.org");
    auto instance = skyr::parse(input);
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("https://example.org/" == output);
  }

  SECTION("url_serialize_2") {
    auto instance = skyr::parse("https://////example.com///");
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("https://example.com///" == output);
  }

  SECTION("url_serialize_3") {
    auto instance = skyr::parse("https://example.com/././foo");
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("https://example.com/foo" == output);
  }

  SECTION("url_serialize_4") {
    auto base = skyr::parse("https://example.com/");
    REQUIRE(base);
    auto instance = skyr::parse("hello:world", base.value());
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("hello:world" == output);
  }

  SECTION("url_serialize_5") {
    auto base = skyr::parse("https://example.com/");
    REQUIRE(base);
    auto instance = skyr::parse("https:example.org", base.value());
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("https://example.com/example.org" == output);
  }

  SECTION("url_serialize_6") {
    auto base = skyr::parse("https://example.com/");
    REQUIRE(base);
    auto instance = skyr::parse("\\example\\..\\demo/.\\", base.value());
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("https://example.com/demo/" == output);
  }

  SECTION("url_serialize_7") {
    auto base = skyr::parse("https://example.com/demo");
    REQUIRE(base);
    auto instance = skyr::parse("example", base.value());
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("https://example.com/example" == output);
  }

  SECTION("url_serialize_8") {
    auto instance = skyr::parse("file:///C|/demo");
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("file:///C:/demo" == output);
  }

  SECTION("url_serialize_9") {
    auto base = skyr::parse("file:///C:/demo");
    REQUIRE(base);
    auto instance = skyr::parse("..", base.value());
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("file:///C:/" == output);
  }

  SECTION("url_serialize_10") {
    auto instance = skyr::parse("file://loc%61lhost/");
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("file:///" == output);
  }

  SECTION("url_serialize_11") {
  auto instance = skyr::parse("https://user:password@example.org/");
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("https://user:password@example.org/" == output);
  }

  SECTION("url_serialize_12") {
    auto instance = skyr::parse("https://example.org/foo bar");
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("https://example.org/foo%20bar" == output);
  }

  SECTION("url_serialize_13") {
    auto instance = skyr::parse("https://EXAMPLE.com/../x");
    REQUIRE(instance);
    auto output = skyr::serialize(instance.value());
    CHECK("https://example.com/x" == output);
  }

  SECTION("url_parse_fails_1") {
    auto instance = skyr::parse("https://ex ample.org/");
    REQUIRE_FALSE(instance);
  }

  SECTION("url_parse_fails_2") {
    auto instance = skyr::parse("example");
    REQUIRE_FALSE(instance);
  }

  SECTION("url_parse_fails_3") {
    auto instance = skyr::parse("https://example.com:demo");
    REQUIRE_FALSE(instance);
  }

  SECTION("url_parse_fails_4") {
    auto instance = skyr::parse("http://[www.example.com]/");
    REQUIRE_FALSE(instance);
  }

  SECTION("test_protocol_non_special_to_special") {
    auto instance = skyr::parse("non-special://example.com/");
    REQUIRE(instance);
    CHECK("non-special://example.com/" == skyr::serialize(instance.value()));
  }
}
