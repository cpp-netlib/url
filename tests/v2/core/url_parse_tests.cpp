// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_all.hpp>
#include <skyr/v2/core/parse.hpp>
#include <skyr/v2/core/serialize.hpp>

/// https://url.spec.whatwg.org/#example-url-parsing

TEST_CASE("url_parsing_example_tests", "[parse]") {
  using namespace std::string_literals;

  SECTION("url_host_0") {
    auto instance = skyr::parse("https://example.org/");
    REQUIRE(instance);
    CHECK(instance.value().host.value().serialize() == "example.org");
  }

  SECTION("url_host_0_with_validation_error") {
    bool validation_error = false;
    auto instance = skyr::parse("https://example.org/", &validation_error);
    REQUIRE(instance);
    CHECK_FALSE(validation_error);
    CHECK(instance.value().host.value().serialize() == "example.org");
  }

  SECTION("url_host_1") {
    auto instance = skyr::parse("https:example.org");
    REQUIRE(instance);
    CHECK(instance.value().host.value().serialize() == "example.org");
  }

  SECTION("url_host_1_with_validation_error") {
    bool validation_error = false;
    auto instance = skyr::parse("https:example.org", &validation_error);
    REQUIRE(instance);
    CHECK(validation_error);
    CHECK(instance.value().host.value().serialize() == "example.org");
  }

  SECTION("url_host_1") {
    auto instance = skyr::parse("https://example.org");
    REQUIRE(instance);
    CHECK(1 == instance.value().path.size());
  }

  SECTION("url_host_2") {
    auto instance = skyr::parse("https://////example.com///");
    REQUIRE(instance);
    CHECK(3 == instance.value().path.size());
  }

  SECTION("url_path_2_with_validation_error") {
    bool validation_error = false;
    auto instance = skyr::parse("https://////example.com///", &validation_error);
    REQUIRE(instance);
    CHECK(3 == instance.value().path.size());
    CHECK(validation_error);
  }

  SECTION("url_parse_fails_1") {
    auto instance = skyr::parse("https://ex ample.org/");
    REQUIRE_FALSE(instance);
  }

  SECTION("url_parse_fails_1_with_validation_error") {
    bool validation_error = false;
    auto instance = skyr::parse("https://ex ample.org/", &validation_error);
    REQUIRE_FALSE(instance);
    CHECK(validation_error);
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
}
