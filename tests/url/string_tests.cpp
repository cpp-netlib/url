// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <memory>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <skyr/v1/ranges/string_element_range.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

TEST_CASE("split_string_tests", "[url.string.split]") {
  SECTION("split empty string") {
    auto instance = ""sv;
    auto range = skyr::split(instance, "."sv);
    CHECK(range.empty());
  }

  SECTION("split string") {
    auto instance = "x.y"sv;
    auto range = skyr::split(instance, "."sv);
    CHECK(range.size() == 2);
  }

  SECTION("split string with empty elements") {
    auto instance = "x..y"sv;
    auto range = skyr::split(instance, "."sv);
    CHECK(range.size() == 3);
  }

  SECTION("split string with two empty elements") {
    auto instance = "."sv;
    auto range = skyr::split(instance, "."sv);
    CHECK(range.size() == 2);
  }

  SECTION("split string with one empty and one non-empty element") {
    auto instance = ".y"sv;
    auto range = skyr::split(instance, "."sv);
    CHECK(range.size() == 2);
  }

  SECTION("split string with one non-empty and one empty element") {
    auto instance = "x."sv;
    auto range = skyr::split(instance, "."sv);
    CHECK(range.size() == 2);
  }

  SECTION("split string with three empty elements") {
    auto instance = ".."sv;
    auto range = skyr::split(instance, "."sv);
    CHECK(range.size() == 3);
  }
}
