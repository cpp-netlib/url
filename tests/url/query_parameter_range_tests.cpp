// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <string_view>
#include <skyr/query/query_parameter_range.hpp>

TEST_CASE("query_parameter_range_test", "[query_parameter_range]") {
  using namespace std::string_view_literals;

  SECTION("empty_query") {
    auto query = ""sv;
    auto range = skyr::query_parameter_range(query);
    CHECK(range.empty());
  }

  SECTION("query_with_single_parameter") {
    auto query = "a=b"sv;
    auto range = skyr::query_parameter_range(query);
    CHECK_FALSE(range.empty());
    CHECK(1 == range.size());
  }

  SECTION("query_with_two_parameters") {
    auto query = "a=b&c=d"sv;
    auto range = skyr::query_parameter_range(query);
    CHECK_FALSE(range.empty());
    CHECK(2 == range.size());
  }

  SECTION("query_with_two_parameters_and_semicolon_separator") {
    auto query = "a=b;c=d"sv;
    auto range = skyr::query_parameter_range(query);
    CHECK_FALSE(range.empty());
    CHECK(2 == range.size());
  }

  SECTION("query_with_no_separators") {
    auto query = "query"sv;
    auto range = skyr::query_parameter_range(query);
    CHECK_FALSE(range.empty());
    CHECK(1 == range.size());
  }

  SECTION("iterator over query range") {
    auto query = "query"sv;
    for (auto [name, value] : skyr::query_parameter_range(query)) {
      CHECK("query" == name);
      CHECK_FALSE(value);
    }
  }
}
