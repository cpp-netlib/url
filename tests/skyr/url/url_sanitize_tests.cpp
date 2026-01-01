// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <exception>

#include <catch2/catch_all.hpp>

#include <skyr/url.hpp>

TEST_CASE("url_sanitize_tests", "[url]") {
  using namespace std::string_literals;

  SECTION("sanitize_removes_credentials") {
    auto url = skyr::url("https://user:pass@example.com/path?query=1#fragment");
    auto sanitized = url.sanitize();

    CHECK(sanitized.username().empty());
    CHECK(sanitized.password().empty());
    CHECK(sanitized.hash().empty());
    CHECK(sanitized.search() == "?query=1");
    CHECK(sanitized.pathname() == "/path");
    CHECK(sanitized.hostname() == "example.com");
    CHECK(sanitized.href() == "https://example.com/path?query=1");
  }

  SECTION("sanitize_removes_fragment") {
    auto url = skyr::url("https://example.com/path#section");
    auto sanitized = url.sanitize();

    CHECK(sanitized.hash().empty());
    CHECK(sanitized.href() == "https://example.com/path");
  }

  SECTION("without_query") {
    auto url = skyr::url("https://example.com/path?foo=1&bar=2#fragment");
    auto result = url.without_query();

    CHECK(result.search().empty());
    CHECK(result.hash() == "#fragment");
    CHECK(result.pathname() == "/path");
    CHECK(result.href() == "https://example.com/path#fragment");
  }

  SECTION("without_fragment") {
    auto url = skyr::url("https://example.com/path?query=1#fragment");
    auto result = url.without_fragment();

    CHECK(result.hash().empty());
    CHECK(result.search() == "?query=1");
    CHECK(result.href() == "https://example.com/path?query=1");
  }

  SECTION("sanitize_then_without_query") {
    auto url = skyr::url("https://user:pass@example.com/path?query=1#fragment");
    auto sanitized = url.sanitize().without_query();

    CHECK(sanitized.username().empty());
    CHECK(sanitized.password().empty());
    CHECK(sanitized.hash().empty());
    CHECK(sanitized.search().empty());
    CHECK(sanitized.href() == "https://example.com/path");
  }

  SECTION("sanitize_already_clean_url") {
    auto url = skyr::url("https://example.com/path");
    auto sanitized = url.sanitize();

    CHECK(sanitized.href() == url.href());
  }

  SECTION("sanitize_preserves_port") {
    auto url = skyr::url("https://user:pass@example.com:8080/path#fragment");
    auto sanitized = url.sanitize();

    CHECK(sanitized.port() == "8080");
    CHECK(sanitized.href() == "https://example.com:8080/path");
  }

  SECTION("sanitize_is_immutable") {
    auto url = skyr::url("https://user:pass@example.com/path#fragment");
    auto sanitized = url.sanitize();

    // Original should be unchanged
    CHECK(url.username() == "user");
    CHECK(url.password() == "pass");
    CHECK(url.hash() == "#fragment");

    // Sanitized should be clean
    CHECK(sanitized.username().empty());
    CHECK(sanitized.password().empty());
    CHECK(sanitized.hash().empty());
  }

  SECTION("without_params_removes_single_param") {
    auto url = skyr::url("https://example.com/path?foo=1&bar=2&baz=3");
    auto result = url.without_params({"bar"});

    CHECK(result.search_parameters().contains("foo"));
    CHECK_FALSE(result.search_parameters().contains("bar"));
    CHECK(result.search_parameters().contains("baz"));
  }

  SECTION("without_params_removes_multiple_params") {
    auto url = skyr::url("https://example.com/path?foo=1&bar=2&baz=3&qux=4");
    auto result = url.without_params({"bar", "qux"});

    CHECK(result.search_parameters().contains("foo"));
    CHECK_FALSE(result.search_parameters().contains("bar"));
    CHECK(result.search_parameters().contains("baz"));
    CHECK_FALSE(result.search_parameters().contains("qux"));
  }

  SECTION("without_params_nonexistent_param") {
    auto url = skyr::url("https://example.com/path?foo=1");
    auto result = url.without_params({"bar", "baz"});

    CHECK(result.search_parameters().contains("foo"));
    CHECK(result.href() == url.href());
  }

  SECTION("without_params_empty_list") {
    auto url = skyr::url("https://example.com/path?foo=1&bar=2");
    auto result = url.without_params({});

    CHECK(result.href() == url.href());
  }

  SECTION("without_params_removes_all_params") {
    auto url = skyr::url("https://example.com/path?foo=1&bar=2");
    auto result = url.without_params({"foo", "bar"});

    CHECK(result.search().empty());
    CHECK(result.href() == "https://example.com/path");
  }

  SECTION("without_params_is_immutable") {
    auto url = skyr::url("https://example.com/path?foo=1&bar=2&baz=3");
    auto result = url.without_params({"bar"});

    // Original should be unchanged
    CHECK(url.search_parameters().contains("foo"));
    CHECK(url.search_parameters().contains("bar"));
    CHECK(url.search_parameters().contains("baz"));

    // Result should have bar removed
    CHECK(result.search_parameters().contains("foo"));
    CHECK_FALSE(result.search_parameters().contains("bar"));
    CHECK(result.search_parameters().contains("baz"));
  }

  SECTION("without_params_with_duplicate_params") {
    auto url = skyr::url("https://example.com/path?foo=1&foo=2&bar=3");
    auto result = url.without_params({"foo"});

    CHECK_FALSE(result.search_parameters().contains("foo"));
    CHECK(result.search_parameters().contains("bar"));
  }

  SECTION("combined_sanitize_and_without_params") {
    auto url = skyr::url("https://user:pass@example.com/path?foo=1&bar=2&baz=3#fragment");
    auto result = url.sanitize().without_params({"bar"});

    CHECK(result.username().empty());
    CHECK(result.password().empty());
    CHECK(result.hash().empty());
    CHECK(result.search_parameters().contains("foo"));
    CHECK_FALSE(result.search_parameters().contains("bar"));
    CHECK(result.search_parameters().contains("baz"));
    CHECK(result.href() == "https://example.com/path?foo=1&baz=3");
  }

  SECTION("without_query_is_immutable") {
    auto url = skyr::url("https://example.com/path?foo=1");
    auto result = url.without_query();

    CHECK(url.search() == "?foo=1");
    CHECK(result.search().empty());
  }

  SECTION("without_fragment_is_immutable") {
    auto url = skyr::url("https://example.com/path#fragment");
    auto result = url.without_fragment();

    CHECK(url.hash() == "#fragment");
    CHECK(result.hash().empty());
  }

  SECTION("chain_multiple_without_operations") {
    auto url = skyr::url("https://user:pass@example.com/path?foo=1&bar=2#fragment");
    auto result = url.without_query().without_fragment();

    CHECK(result.search().empty());
    CHECK(result.hash().empty());
    CHECK(result.username() == "user");  // Credentials preserved
    CHECK(result.href() == "https://user:pass@example.com/path");
  }

  SECTION("fully_clean_url") {
    auto url = skyr::url("https://user:pass@example.com/path?foo=1&bar=2#fragment");
    auto result = url.sanitize().without_query();

    CHECK(result.username().empty());
    CHECK(result.password().empty());
    CHECK(result.search().empty());
    CHECK(result.hash().empty());
    CHECK(result.href() == "https://example.com/path");
  }
}
