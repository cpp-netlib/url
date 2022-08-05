// Copyright 2017-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_all.hpp>
#include <skyr/v1/url.hpp>
#include <skyr/v1/url_search_parameters.hpp>

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

  SECTION("list of pairs") {
    auto parameters = skyr::url_search_parameters{{"key", "730d67"}};
    REQUIRE("key=730d67" == parameters.to_string());
  }

  SECTION("to_string_test") {
    auto parameters = skyr::url_search_parameters{"key=730d67"};
    REQUIRE("key=730d67" == parameters.to_string());
  }
}

TEST_CASE("url") {
  SECTION("url_with_no_query_test") {
    auto instance = skyr::url("https://example.com/");
    auto &parameters = instance.search_parameters();
    CHECK(parameters.to_string().empty());
  }

  SECTION("url_with_empty_query_test") {
    auto instance = skyr::url("https://example.com/?");
    auto &parameters = instance.search_parameters();
    CHECK(parameters.to_string().empty());
  }

  SECTION("url_test") {
    auto instance = skyr::url("https://example.com/?a=b&c=d");
    auto &parameters = instance.search_parameters();
    CHECK("a=b&c=d" == parameters.to_string());
  }

  SECTION("search_parameters_test_1") {
    auto instance = skyr::url("https://example.com/");
    auto &search = instance.search_parameters();
    CHECK(search.empty());
    CHECK(search.to_string().empty());
    CHECK(instance.search().empty());
  }

  SECTION("search_parameters_test_2") {
    auto instance = skyr::url("https://example.com/?");
    auto &search = instance.search_parameters();
    CHECK(search.empty());
    CHECK(search.to_string().empty());
    CHECK(instance.search().empty());
  }

  SECTION("search_parameters_test_3") {
    auto instance = skyr::url("https://example.com/?a=b&c=d");
    auto &search = instance.search_parameters();
    CHECK("a=b&c=d" == search.to_string());
    CHECK("?a=b&c=d" == instance.search());
    CHECK("a=b&c=d" == instance.record().query);
  }

  SECTION("search_parameters_test_4") {
    auto instance = skyr::url("https://example.com/?a=b&c=d");
    auto &search = instance.search_parameters();
    search.set("e", "f");
    CHECK("a=b&c=d&e=f" == search.to_string());
    CHECK("?a=b&c=d&e=f" == instance.search());
    CHECK("a=b&c=d&e=f" == instance.record().query);
  }

  SECTION("search_parameters_test_5") {
    auto instance = skyr::url("https://example.com/?a=b&c=d");
    auto &search = instance.search_parameters();
    search.set("a", "e");
    CHECK("a=e&c=d" == search.to_string());
    CHECK("?a=e&c=d" == instance.search());
    CHECK("a=e&c=d" == instance.record().query);
  }

  SECTION("search_parameters_test_6") {
    auto instance = skyr::url("https://example.com/?c=b&a=d");
    auto &search = instance.search_parameters();
    search.sort();
    CHECK("a=d&c=b" == search.to_string());
    CHECK("?a=d&c=b" == instance.search());
    CHECK("a=d&c=b" == instance.record().query);
  }

  SECTION("search_parameters_test_7") {
    auto instance = skyr::url("https://example.com/?c=b&a=d");
    auto &search = instance.search_parameters();
    search.clear();
    CHECK(search.empty());
    CHECK(search.to_string().empty());
    CHECK(instance.search().empty());
  }

  SECTION("url_search_parameters") {
    // https://url.spec.whatwg.org/#example-searchparams-sort
    auto url = skyr::url(
        "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
    url.search_parameters().sort();
    CHECK("key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.search_parameters().to_string());
    CHECK("?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.search());
    CHECK("https://example.org/?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.href());
  }

  SECTION("url_search_parameters from factory") {
    // https://url.spec.whatwg.org/#example-searchparams-sort
    auto url = skyr::make_url(
        "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78").value();
    url.search_parameters().sort();
    CHECK("key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.search_parameters().to_string());
    CHECK("?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.search());
    CHECK("https://example.org/?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.href());
  }

  SECTION("url_search_parameters from move") {
    // https://url.spec.whatwg.org/#example-searchparams-sort
    auto url = skyr::url(
        "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
    auto new_url(std::move(url));
    new_url.search_parameters().sort();
    CHECK("key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == new_url.search_parameters().to_string());
    CHECK("?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == new_url.search());
    CHECK("https://example.org/?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == new_url.href());
  }

  SECTION("url_search_parameters from copy") {
    // https://url.spec.whatwg.org/#example-searchparams-sort
    auto url = skyr::url(
        "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
    auto new_url(url);
    new_url.search_parameters().sort();
    CHECK("key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == new_url.search_parameters().to_string());
    CHECK("?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == new_url.search());
    CHECK("https://example.org/?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == new_url.href());
  }

  SECTION("url_search_parameters_from_u32_string") {
    // https://url.spec.whatwg.org/#example-searchparams-sort
    auto url = skyr::url(
        U"https://example.org/?q=\x1F3F3\xFE0F\x200D\x1F308&key=e1f7bc78");
    url.search_parameters().sort();
    CHECK("?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.search());
  }

  SECTION("url_search_parameters_02") {
    // https://url.spec.whatwg.org/#example-searchparams-sort
    auto url = skyr::url(
        "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
    auto sorted = skyr::url_search_parameters(url.search());
    sorted.sort();
    CHECK("key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == sorted.to_string());
  }

  SECTION("url_search_parameters_and_range") {
    // https://url.spec.whatwg.org/#example-searchparams-sort
    auto url = skyr::url(
        "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
    url.search_parameters().sort();

    auto first = std::begin(url.search_parameters()), last = std::end(url.search_parameters());
    REQUIRE(first != last);
    CHECK("key" == first->first);
    ++first;
    REQUIRE(first != last);
    CHECK("q" == first->first);
    ++first;
    REQUIRE(first == last);
  }

  SECTION("url_swap") {
    auto url = skyr::url("https://example.com/?a=b&c=d");
    auto instance = skyr::url();
    url.swap(instance);

    CHECK("?a=b&c=d" == instance.search());
    CHECK("a=b&c=d" == instance.record().query.value());
    CHECK("a=b&c=d" == instance.search_parameters().to_string());

    CHECK(url.search().empty());
    CHECK(!url.record().query);
    CHECK(url.search_parameters().to_string().empty());
  }

  SECTION("url_swap_and_modify") {
    auto url = skyr::url("https://example.com/?a=b&c=d");
    auto instance = skyr::url();
    url.swap(instance);

    auto &parameters = instance.search_parameters();
    parameters.remove("a");
    CHECK("c=d" == parameters.to_string());
    CHECK("?c=d" == instance.search());
    CHECK("c=d" == instance.record().query.value());
  }

  SECTION("test_percent_decoding") {
    auto url = skyr::url(
        "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
    auto value = url.search_parameters().get("q");
    CHECK(value);
    CHECK(value.value() == "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    CHECK("?q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88&key=e1f7bc78" == url.search());
  }

  SECTION("test_percent_decoding_setter") {
    auto url = skyr::url(
        "https://example.org/?key=e1f7bc78");
    url.search_parameters().set("q", "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto value = url.search_parameters().get("q");
    CHECK(value);
    CHECK(value.value() == "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    CHECK("?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.search());
  }

  SECTION("test_percent_decoding_append") {
    auto url = skyr::url(
        "https://example.org/?key=e1f7bc78");
    url.search_parameters().append("q", "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto value = url.search_parameters().get("q");
    CHECK(value);
    CHECK(value.value() == "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    CHECK("?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == url.search());
  }
}
