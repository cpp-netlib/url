// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/url.hpp>

TEST_CASE("url_setter_tests", "[url]") {
  SECTION("test_href_1") {
    auto instance = skyr::url{};
  
    auto result = instance.set_href("http://example.com/");
    REQUIRE(result);
    CHECK("http:" == instance.protocol());
    CHECK("example.com" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("test_href_2") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_href("https://cpp-netlib.org/?a=b#fragment");
    REQUIRE(result);
    CHECK("https:" == instance.protocol());
    CHECK("cpp-netlib.org" == instance.host());
    CHECK("/" == instance.pathname());
    CHECK("?a=b" == instance.search());
    CHECK("#fragment" == instance.hash());
  }
  
  SECTION("test_href_parse_error") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_href("Ceci n'est pas un URL");
    REQUIRE_FALSE(result);
    CHECK("http:" == instance.protocol());
    CHECK("example.com" == instance.host());
    CHECK("/" == instance.pathname());
    CHECK("" == instance.search());
    CHECK("" == instance.hash());
  }
  
  SECTION("test_protocol_special_to_special") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_protocol("ws");
    REQUIRE(result);
    CHECK("ws://example.com/" == instance.href());
    CHECK("ws:" == instance.protocol());
  }
  
  SECTION("test_protocol_special_to_non_special") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_protocol("non-special");
    REQUIRE_FALSE(result);
    CHECK("http://example.com/" == instance.href());
  }
  
  SECTION("test_protocol_non_special_to_special") {
    auto instance = skyr::url{"non-special://example.com/"};
  
    auto result = instance.set_protocol("http");
    REQUIRE_FALSE(result);
    CHECK("non-special://example.com/" == instance.href());
  }
  
  SECTION("test_protocol_has_port_to_file") {
    auto instance = skyr::url{"http://example.com:8080/"};
  
    auto result = instance.set_protocol("file");
    REQUIRE_FALSE(result);
    CHECK("http://example.com:8080/" == instance.href());
  }
  
  SECTION("test_protocol_has_no_port_to_file") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_protocol("file");
    REQUIRE(result);
    CHECK("file://example.com/" == instance.href());
    CHECK("file:" == instance.protocol());
  }
  
  SECTION("test_username") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_username("user");
    REQUIRE(result);
    CHECK("http://user@example.com/" == instance.href());
    CHECK("user" == instance.username());
  }
  
  SECTION("test_username_pct_encoded") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_username("us er");
    REQUIRE(result);
    CHECK("http://us%20er@example.com/" == instance.href());
    CHECK("us%20er" == instance.username());
  }
  
  SECTION("test_username_file_scheme") {
    auto instance = skyr::url{"file://example.com/"};
  
    auto result = instance.set_username("user");
    REQUIRE_FALSE(result);
    CHECK("file://example.com/" == instance.href());
  }
  
  SECTION("test_password") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_password("pass");
    REQUIRE(result);
    CHECK("http://:pass@example.com/" == instance.href());
    CHECK("pass" == instance.password());
  }
  
  SECTION("test_password_pct_encoded") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_password("pa ss");
    REQUIRE(result);
    CHECK("http://:pa%20ss@example.com/" == instance.href());
    CHECK("pa%20ss" == instance.password());
  }
  
  SECTION("test_password_file_scheme") {
    auto instance = skyr::url{"file://example.com/"};
  
    auto result = instance.set_password("pass");
    REQUIRE_FALSE(result);
    CHECK("file://example.com/" == instance.href());
  }
  
  SECTION("test_host_http") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_host("elpmaxe.com");
    REQUIRE(result);
    CHECK("http://elpmaxe.com/" == instance.href());
    CHECK("elpmaxe.com" == instance.host());
  }
  
  SECTION("test_host_with_port_number") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_host("elpmaxe.com:8080");
    REQUIRE(result);
    CHECK("http://elpmaxe.com:8080/" == instance.href());
    CHECK("elpmaxe.com:8080" == instance.host());
    CHECK("elpmaxe.com" == instance.hostname());
  }
  
  SECTION("test_host_file_set_non_empty_host") {
    auto instance = skyr::url{"file:///path/to/helicon/"};
  
    auto result = instance.set_host("example.com");
    REQUIRE(result);
    CHECK("file://example.com/path/to/helicon/" == instance.href());
    CHECK("example.com" == instance.host());
    CHECK("/path/to/helicon/" == instance.pathname());
  }
  
  SECTION("test_host_file_with_port_number") {
    auto instance = skyr::url{"file:///path/to/helicon/"};
  
    auto result = instance.set_host("example.com:8080");
    REQUIRE_FALSE(result);
  }
  
  SECTION("test_host_file_set_empty_host") {
    auto instance = skyr::url{"file://example.com/path/to/helicon/"};
  
    auto result = instance.set_host("");
    REQUIRE(result);
    CHECK("file:///path/to/helicon/" == instance.href());
    CHECK("" == instance.host());
    CHECK("/path/to/helicon/" == instance.pathname());
  }
  
  
  SECTION("test_hostname_http") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_hostname("elpmaxe.com");
    REQUIRE(result);
    CHECK("http://elpmaxe.com/" == instance.href());
    CHECK("elpmaxe.com" == instance.hostname());
  }
  
  SECTION("test_hostname_with_port_number") {
    auto instance = skyr::url{"http://example.com:8080/"};
  
    auto result = instance.set_hostname("elpmaxe.com");
    REQUIRE(result);
    CHECK("http://elpmaxe.com:8080/" == instance.href());
    CHECK("elpmaxe.com:8080" == instance.host());
    CHECK("elpmaxe.com" == instance.hostname());
  }
  
  SECTION("test_hostname_file_set_non_empty_host") {
    auto instance = skyr::url{"file:///path/to/helicon/"};
  
    auto result = instance.set_hostname("example.com");
    REQUIRE(result);
    CHECK("file://example.com/path/to/helicon/" == instance.href());
    CHECK("example.com" == instance.hostname());
    CHECK("/path/to/helicon/" == instance.pathname());
  }
  
  SECTION("test_hostname_file_set_empty_host") {
    auto instance = skyr::url{"file://example.com/path/to/helicon/"};
  
    auto result = instance.set_hostname("");
    REQUIRE(result);
    CHECK("file:///path/to/helicon/" == instance.href());
    CHECK("" == instance.hostname());
    CHECK("/path/to/helicon/" == instance.pathname());
  }
  
  SECTION("test_port_no_port") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_port("8080");
    REQUIRE(result);
    CHECK("http://example.com:8080/" == instance.href());
  }
  
  SECTION("test_port_existing_port") {
    auto instance = skyr::url{"http://example.com:80/"};
  
    auto result = instance.set_port("8080");
    REQUIRE(result);
    CHECK("http://example.com:8080/" == instance.href());
  }
  
  SECTION("test_port_existing_port_no_port_1") {
    auto instance = skyr::url{"http://example.com:80/"};
  
    auto result = instance.set_port("");
    REQUIRE(result);
    CHECK("http://example.com/" == instance.href());
  }
  
  SECTION("test_port_invalid_port_1") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_port("Ceci n'est pas un port");
    REQUIRE_FALSE(result);
    CHECK("http://example.com/" == instance.href());
  }
  
  SECTION("test_port_invalid_port_2") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_port("1234567890");
    REQUIRE_FALSE(result);
    CHECK("http://example.com/" == instance.href());
  }
  
  SECTION("test_port_invalid_port_3") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_port("8080C");
    REQUIRE_FALSE(result);
    CHECK("http://example.com/" == instance.href());
  }
  
  SECTION("test_port_invalid_port_4") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_port("-1");
    REQUIRE_FALSE(result);
    CHECK("http://example.com/" == instance.href());
  }
  
  SECTION("test_port_existing_port_no_port_2") {
    auto instance = skyr::url{"http://example.com:/"};
  
    auto result = instance.set_port("");
    REQUIRE(result);
    CHECK("http://example.com/" == instance.href());
  }
  
  SECTION("test_port_no_port_int") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_port(8080);
    REQUIRE(result);
    CHECK("http://example.com:8080/" == instance.href());
  }
  
  SECTION("test_pathname_1") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_pathname("/path/to/helicon/");
    REQUIRE(result);
    CHECK("http://example.com/path/to/helicon/" == instance.href());
  }
  
  SECTION("test_pathname_2") {
    auto instance = skyr::url{"http://example.com/path/to/helicon/"};
  
    auto result = instance.set_pathname("");
    REQUIRE(result);
    CHECK("http://example.com/" == instance.href());
  }
  
  SECTION("test_pathname_3") {
    auto instance = skyr::url{"file:///path/to/helicon/"};
  
    auto result = instance.set_pathname("");
    REQUIRE(result);
    CHECK("file:///" == instance.href());
  }
  
  SECTION("test_search_1") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_search("?a=b&c=d");
    REQUIRE(result);
    CHECK("http://example.com/?a=b&c=d" == instance.href());
  }
  
  SECTION("test_search_2") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_search("a=b&c=d");
    REQUIRE(result);
    CHECK("http://example.com/?a=b&c=d" == instance.href());
  }
  
  SECTION("test_search_3") {
    auto instance = skyr::url{"http://example.com/#fragment"};
  
    auto result = instance.set_search("?a=b&c=d");
    REQUIRE(result);
    CHECK("http://example.com/?a=b&c=d#fragment" == instance.href());
  }
  
  SECTION("test_hash_1") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_hash("#fragment");
    REQUIRE(result);
    CHECK("http://example.com/#fragment" == instance.href());
  }
  
  SECTION("test_hash_2") {
    auto instance = skyr::url{"http://example.com/"};
  
    auto result = instance.set_hash("fragment");
    REQUIRE(result);
    CHECK("http://example.com/#fragment" == instance.href());
  }
}
