// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <exception>

#include <catch2/catch_all.hpp>

#include <skyr/url.hpp>

TEST_CASE("url_with_methods_tests", "[url][with_methods]") {
  using namespace std::string_literals;

  // Basic Functionality Tests

  SECTION("with_scheme_success") {
    auto url = skyr::url("http://example.com/path");
    auto result = url.with_scheme("https");

    REQUIRE(result.has_value());
    CHECK(result->scheme() == "https");
    CHECK(result->href() == "https://example.com/path");
    CHECK(url.scheme() == "http");  // Original unchanged
  }

  SECTION("with_hostname_success") {
    auto url = skyr::url("http://example.com:8080/path");
    auto result = url.with_hostname("newhost.com");

    REQUIRE(result.has_value());
    CHECK(result->hostname() == "newhost.com");
    CHECK(result->port() == "8080");  // Port preserved
    CHECK(result->href() == "http://newhost.com:8080/path");
  }

  SECTION("with_port_integer") {
    auto url = skyr::url("http://example.com/");
    auto result = url.with_port(8080);

    REQUIRE(result.has_value());
    CHECK(result->port() == "8080");
    CHECK(result->href() == "http://example.com:8080/");
  }

  SECTION("with_port_string") {
    auto url = skyr::url("http://example.com/");
    auto result = url.with_port("9000");

    REQUIRE(result.has_value());
    CHECK(result->port() == "9000");
  }

  SECTION("with_port_clear") {
    auto url = skyr::url("http://example.com:8080/");
    auto result = url.with_port("");

    REQUIRE(result.has_value());
    CHECK(result->port().empty());
    CHECK(result->href() == "http://example.com/");
  }

  SECTION("with_pathname_success") {
    auto url = skyr::url("http://example.com/old/path");
    auto result = url.with_pathname("/new/path");

    REQUIRE(result.has_value());
    CHECK(result->pathname() == "/new/path");
    CHECK(result->href() == "http://example.com/new/path");
  }

  SECTION("with_search_with_question_mark") {
    auto url = skyr::url("http://example.com/path");
    auto result = url.with_search("?foo=bar");

    REQUIRE(result.has_value());
    CHECK(result->search() == "?foo=bar");
  }

  SECTION("with_search_without_question_mark") {
    auto url = skyr::url("http://example.com/path");
    auto result = url.with_search("foo=bar");

    REQUIRE(result.has_value());
    CHECK(result->search() == "?foo=bar");
  }

  SECTION("with_hash_with_hash_mark") {
    auto url = skyr::url("http://example.com/path");
    auto result = url.with_hash("#section");

    REQUIRE(result.has_value());
    CHECK(result->hash() == "#section");
  }

  SECTION("with_hash_without_hash_mark") {
    auto url = skyr::url("http://example.com/path");
    auto result = url.with_hash("section");

    REQUIRE(result.has_value());
    CHECK(result->hash() == "#section");
  }

  SECTION("with_username_success") {
    auto url = skyr::url("http://example.com/");
    auto result = url.with_username("user");

    REQUIRE(result.has_value());
    CHECK(result->username() == "user");
    CHECK(result->href() == "http://user@example.com/");
  }

  SECTION("with_password_success") {
    auto url = skyr::url("http://user@example.com/");
    auto result = url.with_password("pass");

    REQUIRE(result.has_value());
    CHECK(result->password() == "pass");
    CHECK(result->href() == "http://user:pass@example.com/");
  }

  SECTION("with_host_success") {
    auto url = skyr::url("http://example.com/path");
    auto result = url.with_host("newhost.com:9000");

    REQUIRE(result.has_value());
    CHECK(result->hostname() == "newhost.com");
    CHECK(result->port() == "9000");
    CHECK(result->href() == "http://newhost.com:9000/path");
  }

  // Error Handling Tests

  SECTION("with_scheme_invalid_transition") {
    auto url = skyr::url("http://example.com/");
    auto result = url.with_scheme("mailto");

    CHECK_FALSE(result.has_value());
  }

  SECTION("with_hostname_cannot_be_base") {
    auto url = skyr::url("mailto:user@example.com");
    auto result = url.with_hostname("other.com");

    CHECK_FALSE(result.has_value());
  }

  SECTION("with_port_invalid_number") {
    auto url = skyr::url("http://example.com/");
    auto result = url.with_port("99999");

    CHECK_FALSE(result.has_value());
  }

  SECTION("with_port_on_file_scheme") {
    auto url = skyr::url("file:///path/to/file");
    auto result = url.with_port(8080);

    CHECK_FALSE(result.has_value());
  }

  SECTION("with_username_on_file_scheme") {
    auto url = skyr::url("file:///path/to/file");
    auto result = url.with_username("user");

    CHECK_FALSE(result.has_value());
  }

  SECTION("with_password_on_file_scheme") {
    auto url = skyr::url("file:///path/to/file");
    auto result = url.with_password("pass");

    CHECK_FALSE(result.has_value());
  }

  // Immutability Tests

  SECTION("with_hostname_is_immutable") {
    auto url = skyr::url("http://example.com/path");
    auto result = url.with_hostname("other.com");

    CHECK(url.hostname() == "example.com");  // Original unchanged
    REQUIRE(result.has_value());
    CHECK(result->hostname() == "other.com");
  }

  SECTION("with_port_is_immutable") {
    auto url = skyr::url("http://example.com:8080/");
    auto result = url.with_port(9000);

    CHECK(url.port() == "8080");
    REQUIRE(result.has_value());
    CHECK(result->port() == "9000");
  }

  SECTION("with_pathname_is_immutable") {
    auto url = skyr::url("http://example.com/old");
    auto result = url.with_pathname("/new");

    CHECK(url.pathname() == "/old");
    REQUIRE(result.has_value());
    CHECK(result->pathname() == "/new");
  }

  SECTION("with_search_is_immutable") {
    auto url = skyr::url("http://example.com/?old=1");
    auto result = url.with_search("new=2");

    CHECK(url.search() == "?old=1");
    REQUIRE(result.has_value());
    CHECK(result->search() == "?new=2");
  }

  SECTION("with_hash_is_immutable") {
    auto url = skyr::url("http://example.com/#old");
    auto result = url.with_hash("new");

    CHECK(url.hash() == "#old");
    REQUIRE(result.has_value());
    CHECK(result->hash() == "#new");
  }

  // Method Chaining Tests

  SECTION("chain_with_and_then") {
    auto url = skyr::url("http://example.com/");

    auto result =
        url.with_hostname("newhost.com").and_then([](auto&& u) { return u.with_port(8080); }).and_then([](auto&& u) {
          return u.with_pathname("/api/v1");
        });

    REQUIRE(result.has_value());
    CHECK(result->href() == "http://newhost.com:8080/api/v1");
  }

  SECTION("chain_stops_on_first_error") {
    auto url = skyr::url("http://example.com/");

    auto result = url.with_port("99999")  // Port out of range fails
                      .and_then([](auto&& u) { return u.with_hostname("other.com"); });

    CHECK_FALSE(result.has_value());
  }

  SECTION("chain_with_transform") {
    auto url = skyr::url("http://example.com/");

    auto result = url.with_hostname("api.example.com").transform([](auto&& u) { return u.href(); });

    REQUIRE(result.has_value());
    CHECK(result.value() == "http://api.example.com/");
  }

  SECTION("chain_multiple_components") {
    auto url = skyr::url("http://localhost:3000/");

    auto result = url.with_scheme("https")
                      .and_then([](auto&& u) { return u.with_hostname("api.production.com"); })
                      .and_then([](auto&& u) { return u.with_port(8443); })
                      .and_then([](auto&& u) { return u.with_pathname("/v2/users"); })
                      .and_then([](auto&& u) { return u.with_search("limit=100"); });

    REQUIRE(result.has_value());
    CHECK(result->href() == "https://api.production.com:8443/v2/users?limit=100");
  }

  // Integration with Existing Immutable Methods

  SECTION("combine_with_sanitize") {
    auto url = skyr::url("http://user:pass@example.com:8080/path#fragment");

    auto result = url.sanitize().with_port(9000);

    REQUIRE(result.has_value());
    CHECK(result->username().empty());
    CHECK(result->password().empty());
    CHECK(result->hash().empty());
    CHECK(result->port() == "9000");
    CHECK(result->href() == "http://example.com:9000/path");
  }

  SECTION("combine_with_without_query") {
    auto url = skyr::url("http://example.com/path?foo=bar#fragment");

    auto result = url.without_query().with_hash("newsection");

    REQUIRE(result.has_value());
    CHECK(result->search().empty());
    CHECK(result->hash() == "#newsection");
    CHECK(result->href() == "http://example.com/path#newsection");
  }

  SECTION("combine_with_without_fragment") {
    auto url = skyr::url("http://example.com/path?query=1#fragment");

    auto result = url.without_fragment().with_pathname("/new");

    REQUIRE(result.has_value());
    CHECK(result->hash().empty());
    CHECK(result->pathname() == "/new");
    CHECK(result->search() == "?query=1");
  }

  SECTION("complex_transformation_chain") {
    auto url = skyr::url("http://user:pass@example.com:8080/old/path?debug=true&id=123#section");

    auto result = url.sanitize()                  // Remove credentials & fragment
                      .without_params({"debug"})  // Remove debug param
                      .with_pathname("/api/v2/data")
                      .and_then([](auto&& u) { return u.with_search("format=json&id=123"); });

    REQUIRE(result.has_value());
    CHECK(result->username().empty());
    CHECK(result->password().empty());
    CHECK(result->hash().empty());
    CHECK(result->pathname() == "/api/v2/data");
    CHECK(result->search() == "?format=json&id=123");
  }

  // Alias Tests

  SECTION("with_query_alias") {
    auto url = skyr::url("http://example.com/");
    auto result1 = url.with_search("foo=bar");
    auto result2 = url.with_query("foo=bar");

    REQUIRE(result1.has_value());
    REQUIRE(result2.has_value());
    CHECK(result1->href() == result2->href());
  }

  SECTION("with_fragment_alias") {
    auto url = skyr::url("http://example.com/");
    auto result1 = url.with_hash("section");
    auto result2 = url.with_fragment("section");

    REQUIRE(result1.has_value());
    REQUIRE(result2.has_value());
    CHECK(result1->href() == result2->href());
  }

  // Unicode Support Tests

  SECTION("with_hostname_utf8") {
    auto url = skyr::url("http://example.com/");
    auto result = url.with_hostname("m\xc3\xbcnchen.de");

    REQUIRE(result.has_value());
    // Should be punycoded
    CHECK(result->hostname() == "xn--mnchen-3ya.de");
  }

  SECTION("with_search_utf8") {
    auto url = skyr::url("http://example.com/");
    auto result = url.with_search("query=caf\xc3\xa9");

    REQUIRE(result.has_value());
    // Should be percent-encoded
    CHECK(result->search() == "?query=caf%C3%A9");
  }

  SECTION("with_pathname_utf8") {
    auto url = skyr::url("http://example.com/");
    auto result = url.with_pathname("/path/caf\xc3\xa9");

    REQUIRE(result.has_value());
    CHECK(result->pathname() == "/path/caf%C3%A9");
  }

  SECTION("with_username_utf8") {
    auto url = skyr::url("http://example.com/");
    // "usér" - é = 0xC3 0xA9 in UTF-8
    auto result = url.with_username("us\xc3\xa9r");

    REQUIRE(result.has_value());
    CHECK(result->username() == "us%C3%A9r");
  }

  // Edge Cases

  SECTION("with_search_clear") {
    auto url = skyr::url("http://example.com/path?foo=bar");
    auto result = url.with_search("");

    REQUIRE(result.has_value());
    CHECK(result->search().empty());
    CHECK(result->href() == "http://example.com/path");
  }

  SECTION("with_hash_clear") {
    auto url = skyr::url("http://example.com/path#section");
    auto result = url.with_hash("");

    REQUIRE(result.has_value());
    CHECK(result->hash().empty());
    CHECK(result->href() == "http://example.com/path");
  }

  SECTION("with_pathname_empty") {
    auto url = skyr::url("http://example.com/path");
    auto result = url.with_pathname("");

    REQUIRE(result.has_value());
    CHECK(result->pathname() == "/");
  }

  SECTION("with_username_clear") {
    auto url = skyr::url("http://user@example.com/");
    auto result = url.with_username("");

    REQUIRE(result.has_value());
    CHECK(result->username().empty());
    CHECK(result->href() == "http://example.com/");
  }

  SECTION("with_password_clear") {
    auto url = skyr::url("http://user:pass@example.com/");
    auto result = url.with_password("");

    REQUIRE(result.has_value());
    CHECK(result->password().empty());
    CHECK(result->href() == "http://user@example.com/");
  }

  // Error Recovery Pattern

  SECTION("error_recovery_with_or_else") {
    auto url = skyr::url("http://example.com/");

    auto result = url.with_port("99999")  // Port out of range will fail
                      .or_else([&url](auto&& error) -> std::expected<skyr::url, std::error_code> {
                        return url.with_port(8080);  // Fallback to default
                      });

    REQUIRE(result.has_value());
    CHECK(result->port() == "8080");
    CHECK(result->href() == "http://example.com:8080/");
  }

  // Real-world Use Cases

  SECTION("build_api_url") {
    auto base_url = skyr::url("https://api.example.com");

    auto api_url =
        base_url.with_pathname("/v1/users").and_then([](auto&& u) { return u.with_search("page=2&limit=50"); });

    REQUIRE(api_url.has_value());
    CHECK(api_url->href() == "https://api.example.com/v1/users?page=2&limit=50");
  }

  SECTION("switch_environment") {
    auto dev_url = skyr::url("http://localhost:3000/api/users");

    auto prod_url = dev_url.with_scheme("https")
                        .and_then([](auto&& u) { return u.with_hostname("api.production.com"); })
                        .and_then([](auto&& u) { return u.with_port(""); });  // Clear non-default port

    REQUIRE(prod_url.has_value());
    CHECK(prod_url->href() == "https://api.production.com/api/users");
  }
}
