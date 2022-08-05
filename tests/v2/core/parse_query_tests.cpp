// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_all.hpp>
#include <skyr/v2/core/parse_query.hpp>

TEST_CASE("query_parsing_example_tests", "[parse]") {
  SECTION("url_query_1") {
    auto query = skyr::parse_query("?a=b");
    REQUIRE(query);
    CHECK(1 == query.value().size());
    CHECK(query.value().front().name == "a");
    CHECK(query.value().front().value);
    CHECK(query.value().front().value.value() == "b");
  }

  SECTION("url_query_2") {
    auto query = skyr::parse_query("c=d");
    REQUIRE(query);
    CHECK(1 == query.value().size());
    CHECK(query.value().front().name == "c");
    CHECK(query.value().front().value);
    CHECK(query.value().front().value.value() == "d");
  }

  SECTION("url_query_3") {
    auto query = skyr::parse_query("?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
    REQUIRE(query);
    CHECK(2 == query.value().size());
    CHECK(query.value()[0].name == "q");
    CHECK(query.value()[0].value);
    CHECK(query.value()[0].value.value() == "%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88");
    CHECK(query.value()[1].name == "key");
    CHECK(query.value()[1].value);
    CHECK(query.value()[1].value.value() == "e1f7bc78");
  }
}
