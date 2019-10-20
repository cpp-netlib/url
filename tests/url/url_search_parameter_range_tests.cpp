// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/url/url_search_parameters.hpp>

TEST_CASE("search_element_iterator_test", "[search_parameter_range]") {
  SECTION("empty_query") {
    auto first = skyr::search_element_iterator(std::string_view("")), last = skyr::search_element_iterator();
    CHECK(first == last);
  }

  SECTION("query_with_single_element") {
    auto search = "a=b";
    auto first = skyr::search_element_iterator(std::string_view(search)), last = skyr::search_element_iterator();
    CHECK(first != last);

    CHECK("a=b" == *first);
    ++first;
    CHECK(first == last);
  }

  SECTION("query_with_two_elements") {
    auto search = "a=b&c=d";
    auto first = skyr::search_element_iterator(std::string_view(search)), last = skyr::search_element_iterator();
    CHECK(first != last);

    CHECK("a=b" == *first);
    ++first;
    CHECK("c=d" == *first);
    ++first;
    CHECK(first == last);
  }

  SECTION("query_with_two_elements_and_semicolon_separator") {
    auto search = "a=b;c=d";
    auto first = skyr::search_element_iterator(std::string_view(search)), last = skyr::search_element_iterator();
    CHECK(first != last);

    CHECK("a=b" == *first);
    ++first;
    CHECK("c=d" == *first);
    ++first;
    CHECK(first == last);
  }
}

TEST_CASE("search_parameter_iterator_test", "[search_parameter_range]") {
  SECTION("empty_query") {
    auto first = skyr::search_parameter_iterator(std::string_view("")), last = skyr::search_parameter_iterator();
    CHECK(first == last);
  }

  SECTION("query_with_single_parameter") {
    auto search = "a=b";
    auto first = skyr::search_parameter_iterator(std::string_view(search)), last = skyr::search_parameter_iterator();
    CHECK(first != last);

    CHECK("a" == (*first).first);
    CHECK("b" == (*first).second);
    ++first;
    CHECK(first == last);
  }

  SECTION("query_with_two_parameters") {
    auto search = "a=b&c=d";
    auto first = skyr::search_parameter_iterator(std::string_view(search)), last = skyr::search_parameter_iterator();
    CHECK(first != last);

    CHECK("a" == (*first).first);
    CHECK("b" == (*first).second);
    ++first;
    CHECK("c" == (*first).first);
    CHECK("d" == (*first).second);
    ++first;
    CHECK(first == last);
  }

  SECTION("query_with_two_parameters_and_semicolon_separator") {
    auto search = "a=b;c=d";
    auto first = skyr::search_parameter_iterator(std::string_view(search)), last = skyr::search_parameter_iterator();
    CHECK(first != last);

    CHECK("a" == (*first).first);
    CHECK("b" == (*first).second);
    ++first;
    CHECK("c" == (*first).first);
    CHECK("d" == (*first).second);
    ++first;
    CHECK(first == last);
  }
}

TEST_CASE("search_parameter_range_test", "[search_parameter_range]") {
  SECTION("empty_query") {
    auto range = skyr::search_parameter_range(std::string_view(""));
    CHECK(range.empty());
  }

  SECTION("query_with_single_parameter") {
    auto search = "a=b";
    auto range = skyr::search_parameter_range(search);
    CHECK(!range.empty());
    CHECK(1 == range.size());
  }

  SECTION("query_with_two_parameters") {
    auto search = "a=b&c=d";
    auto range = skyr::search_parameter_range(search);
    CHECK(!range.empty());
    CHECK(2 == range.size());
  }

  SECTION("query_with_two_parameters_and_semicolon_separator") {
    auto search = "a=b;c=d";
    auto range = skyr::search_parameter_range(search);
    CHECK(!range.empty());
    CHECK(2 == range.size());
  }
}
