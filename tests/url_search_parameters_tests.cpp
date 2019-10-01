// Copyright 2017-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/url.hpp>
#include <skyr/url/url_search_parameters.hpp>
#include <skyr/url/url_parse.hpp>

TEST_CASE("url_search_parameters_test", "[url_search_parameters]") {
  SECTION("empty_query") {
    skyr::url_search_parameters parameters{};
    CHECK("" == parameters.to_string());
    CHECK(parameters.empty());
    CHECK(parameters.begin() == parameters.end());
  }
  
  SECTION("query_with_single_kvp") {
    skyr::url_search_parameters parameters{"a=b"};
  
    CHECK("a=b" == parameters.to_string());
    auto it = parameters.begin();
    REQUIRE_FALSE(it == parameters.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    CHECK(it == parameters.end());
  }
  
  SECTION("query_with_single_kvp_in_initalizer_list") {
    skyr::url_search_parameters parameters{"a=b"};
  
    auto it = parameters.begin();
    REQUIRE_FALSE(it == parameters.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    CHECK(it == parameters.end());
  }
  
  SECTION("query_with_two_kvps") {
    skyr::url_search_parameters parameters{"a=b&c=d"};
  
    auto it = parameters.begin();
    REQUIRE_FALSE(it == parameters.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    REQUIRE_FALSE(it == parameters.end());
    CHECK("c" == it->first);
    CHECK("d" == it->second);
    ++it;
    CHECK(it == parameters.end());
  }
  
  SECTION("query_with_two_kvps_in_initializer_list") {
    skyr::url_search_parameters parameters{"a=b&c=d"};
  
    auto it = parameters.begin();
    REQUIRE_FALSE(it == parameters.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    REQUIRE_FALSE(it == parameters.end());
    CHECK("c" == it->first);
    CHECK("d" == it->second);
    ++it;
    CHECK(it == parameters.end());
  }
  
  SECTION("query_with_two_kvps_using_semicolon_separator") {
    skyr::url_search_parameters parameters{"a=b;c=d"};
  
    CHECK("a=b&c=d" == parameters.to_string());
    auto it = parameters.begin();
    REQUIRE_FALSE(it == parameters.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    REQUIRE_FALSE(it == parameters.end());
    CHECK("c" == it->first);
    CHECK("d" == it->second);
    ++it;
    CHECK(it == parameters.end());
  }
  
  SECTION("query_append_one_kvp") {
    skyr::url_search_parameters parameters{};
    parameters.append("a", "b");
  
    auto it = parameters.begin();
    REQUIRE_FALSE(it == parameters.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    CHECK(it == parameters.end());
  }
  
  SECTION("query_append_two_kvps") {
    skyr::url_search_parameters parameters{};
    parameters.append("a", "b");
    parameters.append("c", "d");
  
    auto it = parameters.begin();
    REQUIRE_FALSE(it == parameters.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    CHECK("c" == it->first);
    CHECK("d" == it->second);
    ++it;
    CHECK(it == parameters.end());
  }
  
  SECTION("query_append_one_kvp_to_string") {
    skyr::url_search_parameters parameters{};
    parameters.append("a", "b");
  
    CHECK("a=b" == parameters.to_string());
  }
  
  SECTION("query_append_two_kvps_to_string") {
    skyr::url_search_parameters parameters{};
    parameters.append("a", "b");
    parameters.append("c", "d");
  
    CHECK("a=b&c=d" == parameters.to_string());
  }
  
  SECTION("query_sort_test") {
    // https://url.spec.whatwg.org/#example-searchparams-sort
    skyr::url_search_parameters parameters{"c=d&a=b"};
    parameters.sort();
    CHECK("a=b&c=d" == parameters.to_string());
  }
  
  SECTION("copy_test") {
    skyr::url_search_parameters parameters{"a=b&c=d"};
    skyr::url_search_parameters copy(parameters);
  
    auto it = copy.begin();
    REQUIRE_FALSE(it == copy.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    CHECK("c" == it->first);
    CHECK("d" == it->second);
    ++it;
    CHECK(it == copy.end());
  }
  
  SECTION("copy_assignment_test") {
    skyr::url_search_parameters parameters{"a=b&c=d"};
    skyr::url_search_parameters copy;
    copy = parameters;
  
    auto it = copy.begin();
    REQUIRE_FALSE(it == copy.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    CHECK("c" == it->first);
    CHECK("d" == it->second);
    ++it;
    CHECK(it == copy.end());
  }
  
  SECTION("move_test") {
    skyr::url_search_parameters parameters{"a=b&c=d"};
    skyr::url_search_parameters copy(std::move(parameters));
  
    auto it = copy.begin();
    REQUIRE_FALSE(it == copy.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    CHECK("c" == it->first);
    CHECK("d" == it->second);
    ++it;
    CHECK(it == copy.end());
  }
  
  SECTION("move_assignment_test") {
    skyr::url_search_parameters parameters{"a=b&c=d"};
    skyr::url_search_parameters copy;
    copy = std::move(parameters);
  
    auto it = copy.begin();
    REQUIRE_FALSE(it == copy.end());
    CHECK("a" == it->first);
    CHECK("b" == it->second);
    ++it;
    CHECK("c" == it->first);
    CHECK("d" == it->second);
    ++it;
    CHECK(it == copy.end());
  }
  
  SECTION("to_string_test") {
    auto parameters = skyr::url_search_parameters{"key=730d67"};
    REQUIRE("key=730d67" == parameters.to_string());
  }
  
  SECTION("url_record_with_no_query_test") {
    auto instance = skyr::parse("https://example.com/");
    REQUIRE(instance);
    auto parameters = skyr::url_search_parameters(instance.value());
    CHECK("" == parameters.to_string());
  }
  
  SECTION("url_record_with_empty_query_test") {
    auto instance = skyr::parse("https://example.com/?");
    REQUIRE(instance);
    auto parameters = skyr::url_search_parameters(instance.value());
    CHECK("" == parameters.to_string());
  }
  
  SECTION("url_record_test") {
    auto instance = skyr::parse("https://example.com/?a=b&c=d");
    REQUIRE(instance);
    auto parameters = skyr::url_search_parameters(instance.value());
    CHECK("a=b&c=d" == parameters.to_string());
  }

#if __cpp_exceptions
  SECTION("url_search_parameters") {
    // https://url.spec.whatwg.org/#example-searchparams-sort
    auto url = skyr::url(
        "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
    url.search_parameters().sort();
    CHECK("?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.search());
  }
  
  SECTION("url_swap") {
    auto url = skyr::url("https://example.com/?a=b&c=d");
    auto instance = skyr::url();
    url.swap(instance);
  
    REQUIRE("?a=b&c=d" == instance.search());
    REQUIRE("" == url.search());
  
    auto parameters = instance.search_parameters();
    CHECK("a=b&c=d" == parameters.to_string());
    parameters.remove("a");
    CHECK("c=d" == parameters.to_string());
    CHECK("?c=d" == instance.search());
    CHECK("c=d" == instance.record().query.value());
    CHECK("" == url.search());
  }
#endif // __cpp_exceptions
}
