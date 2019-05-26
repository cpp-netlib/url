// Copyright 2017-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <skyr/url.hpp>
#include <skyr/url_search_parameters.hpp>
#include <skyr/url_parse.hpp>

TEST(url_search_parameters_test, empty_query) {
  skyr::url_search_parameters parameters{};
  EXPECT_EQ("", parameters.to_string());
  EXPECT_TRUE(parameters.empty());
  EXPECT_EQ(parameters.begin(), parameters.end());
}

TEST(url_search_parameters_test, query_with_single_kvp) {
  skyr::url_search_parameters parameters{"a=b"};

  EXPECT_EQ("a=b", parameters.to_string());
  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(url_search_parameters_test, query_with_single_kvp_in_initalizer_list) {
  skyr::url_search_parameters parameters{"a=b"};

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(url_search_parameters_test, query_with_two_kvps) {
  skyr::url_search_parameters parameters{"a=b&c=d"};

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(url_search_parameters_test, query_with_two_kvps_in_initializer_list) {
  skyr::url_search_parameters parameters{"a=b&c=d"};

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(url_search_parameters_test, query_with_two_kvps_using_semicolon_separator) {
  skyr::url_search_parameters parameters{"a=b;c=d"};

  EXPECT_EQ("a=b&c=d", parameters.to_string());
  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(url_search_parameters_test, query_append_one_kvp) {
  skyr::url_search_parameters parameters{};
  parameters.append("a", "b");

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(url_search_parameters_test, query_append_two_kvps) {
  skyr::url_search_parameters parameters{};
  parameters.append("a", "b");
  parameters.append("c", "d");

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(url_search_parameters_test, query_append_one_kvp_to_string) {
  skyr::url_search_parameters parameters{};
  parameters.append("a", "b");

  EXPECT_EQ("a=b", parameters.to_string());
}

TEST(url_search_parameters_test, query_append_two_kvps_to_string) {
  skyr::url_search_parameters parameters{};
  parameters.append("a", "b");
  parameters.append("c", "d");

  EXPECT_EQ("a=b&c=d", parameters.to_string());
}

TEST(url_search_parameters_test, query_sort_test) {
  // https://url.spec.whatwg.org/#example-searchparams-sort
  skyr::url_search_parameters parameters{"c=d&a=b"};
  parameters.sort();
  EXPECT_EQ("a=b&c=d", parameters.to_string());
}

TEST(url_search_parameters_test, copy_test) {
  skyr::url_search_parameters parameters{"a=b&c=d"};
  skyr::url_search_parameters copy(parameters);

  auto it = copy.begin();
  ASSERT_NE(it, copy.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, copy.end());
}

TEST(url_search_parameters_test, copy_assignment_test) {
  skyr::url_search_parameters parameters{"a=b&c=d"};
  skyr::url_search_parameters copy;
  copy = parameters;

  auto it = copy.begin();
  ASSERT_NE(it, copy.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, copy.end());
}

TEST(url_search_parameters_test, move_test) {
  skyr::url_search_parameters parameters{"a=b&c=d"};
  skyr::url_search_parameters copy(std::move(parameters));

  auto it = copy.begin();
  ASSERT_NE(it, copy.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, copy.end());
}

TEST(url_search_parameters_test, move_assignment_test) {
  skyr::url_search_parameters parameters{"a=b&c=d"};
  skyr::url_search_parameters copy;
  copy = std::move(parameters);

  auto it = copy.begin();
  ASSERT_NE(it, copy.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, copy.end());
}

TEST(url_search_parameters_test, to_string_test) {
  auto parameters = skyr::url_search_parameters{"key=730d67"};
  ASSERT_EQ("key=730d67", parameters.to_string());
}

TEST(url_search_parameters_test, url_record_with_no_query_test) {
  auto instance = skyr::parse("https://example.com/");
  ASSERT_TRUE(instance);
  auto parameters = skyr::url_search_parameters(instance.value());
  EXPECT_EQ("", parameters.to_string());
}

TEST(url_search_parameters_test, url_record_with_empty_query_test) {
  auto instance = skyr::parse("https://example.com/?");
  ASSERT_TRUE(instance);
  auto parameters = skyr::url_search_parameters(instance.value());
  EXPECT_EQ("", parameters.to_string());
}

TEST(url_search_parameters_test, url_record_test) {
  auto instance = skyr::parse("https://example.com/?a=b&c=d");
  ASSERT_TRUE(instance);
  auto parameters = skyr::url_search_parameters(instance.value());
  EXPECT_EQ("a=b&c=d", parameters.to_string());
}

TEST(url_search_parameters_test, url_search_parameters) {
  // https://url.spec.whatwg.org/#example-searchparams-sort
  auto url = skyr::url(
      "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
  url.search_parameters().sort();
  EXPECT_EQ("?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88", url.search());
}

TEST(url_search_parameters_test, url_swap) {
  auto url = skyr::url("https://example.com/?a=b&c=d");
  auto instance = skyr::url();
  url.swap(instance);

  ASSERT_EQ("?a=b&c=d", instance.search());
  ASSERT_EQ("", url.search());
  
  auto parameters = instance.search_parameters();
  EXPECT_EQ("a=b&c=d", parameters.to_string());
  parameters.remove("a");
  EXPECT_EQ("c=d", parameters.to_string());
  EXPECT_EQ("?c=d", instance.search());
  EXPECT_EQ("c=d", instance.record().query.value());
  EXPECT_EQ("", url.search());
}
