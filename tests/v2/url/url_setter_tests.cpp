// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_all.hpp>
#include <skyr/v2/url.hpp>

TEST_CASE("url_setter_tests", "[url]") {
  SECTION("test_href_1") {
    auto instance = skyr::url{};

    auto ec = instance.set_href("http://example.com/");
    CHECK_FALSE(ec);
    CHECK("http:" == instance.protocol());
    CHECK("example.com" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("test_href_2") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_href("https://cpp-netlib.org/?a=b#fragment");
    CHECK_FALSE(ec);
    CHECK("https:" == instance.protocol());
    CHECK("cpp-netlib.org" == instance.host());
    CHECK("/" == instance.pathname());
    CHECK("?a=b" == instance.search());
    CHECK("#fragment" == instance.hash());
  }

  SECTION("test_href_parse_error") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_href("Ceci n'est pas un URL");
    CHECK(ec);
    CHECK("http:" == instance.protocol());
    CHECK("example.com" == instance.host());
    CHECK("/" == instance.pathname());
    CHECK("" == instance.search());
    CHECK("" == instance.hash());
  }

  SECTION("test_protocol_special_to_special") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_protocol("ws");
    CHECK_FALSE(ec);
    CHECK("ws://example.com/" == instance.href());
    CHECK("ws:" == instance.protocol());
  }

  SECTION("test_protocol_special_to_non_special") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_protocol("non-special");
    CHECK(ec);
    CHECK("http://example.com/" == instance.href());
  }

  SECTION("test_protocol_non_special_to_special") {
    auto instance = skyr::url{"non-special://example.com/"};

    auto ec = instance.set_protocol("http");
    CHECK(ec);
    CHECK("non-special://example.com/" == instance.href());
  }

  SECTION("test_protocol_has_port_to_file") {
    auto instance = skyr::url{"http://example.com:8080/"};

    auto ec = instance.set_protocol("file");
    CHECK(ec);
    CHECK("http://example.com:8080/" == instance.href());
  }

  SECTION("test_protocol_has_no_port_to_file") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_protocol("file");
    CHECK_FALSE(ec);
    CHECK("file://example.com/" == instance.href());
    CHECK("file:" == instance.protocol());
  }

  SECTION("test_username") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_username("user");
    CHECK_FALSE(ec);
    CHECK("http://user@example.com/" == instance.href());
    CHECK("user" == instance.username());
  }

  SECTION("test_username_pct_encoded") {
    auto instance = skyr::url{"http://example.com/"};

    auto result = instance.set_username("us er");
    CHECK_FALSE(result);
    CHECK("http://us%20er@example.com/" == instance.href());
    CHECK("us%20er" == instance.username());
  }

  SECTION("test_username_file_scheme") {
    auto instance = skyr::url{"file://example.com/"};

    auto ec = instance.set_username("user");
    CHECK(ec);
    CHECK("file://example.com/" == instance.href());
  }

  SECTION("set_username_with_empty_host") {
    auto instance = skyr::url{"sc:///"};
    auto ec = instance.set_username("x");
    CHECK("sc:///" == instance.href());
  }

  SECTION("test_password") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_password("pass");
    CHECK_FALSE(ec);
    CHECK("http://:pass@example.com/" == instance.href());
    CHECK("pass" == instance.password());
  }

  SECTION("test_password_pct_encoded") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_password("pa ss");
    CHECK_FALSE(ec);
    CHECK("http://:pa%20ss@example.com/" == instance.href());
    CHECK("pa%20ss" == instance.password());
  }

  SECTION("test_password_file_scheme") {
    auto instance = skyr::url{"file://example.com/"};

    auto ec = instance.set_password("pass");
    CHECK(ec);
    CHECK("file://example.com/" == instance.href());
  }

  SECTION("test_host_http") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_host("elpmaxe.com");
    CHECK_FALSE(ec);
    CHECK("http://elpmaxe.com/" == instance.href());
    CHECK("elpmaxe.com" == instance.host());
  }

  SECTION("test_host_with_port_number") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_host("elpmaxe.com:8080");
    CHECK_FALSE(ec);
    CHECK("http://elpmaxe.com:8080/" == instance.href());
    CHECK("elpmaxe.com:8080" == instance.host());
    CHECK("elpmaxe.com" == instance.hostname());
  }

  SECTION("test_host_file_set_non_empty_host") {
    auto instance = skyr::url{"file:///path/to/helicon/"};

    auto ec = instance.set_host("example.com");
    CHECK_FALSE(ec);
    CHECK("file://example.com/path/to/helicon/" == instance.href());
    CHECK("example.com" == instance.host());
    CHECK("/path/to/helicon/" == instance.pathname());
  }

  SECTION("test_host_file_with_port_number") {
    auto instance = skyr::url{"file:///path/to/helicon/"};

    auto ec = instance.set_host("example.com:8080");
    CHECK(ec);
  }

  SECTION("test_host_file_set_empty_host") {
    auto instance = skyr::url{"file://example.com/path/to/helicon/"};

    auto ec = instance.set_host("");
    CHECK_FALSE(ec);
    CHECK("file:///path/to/helicon/" == instance.href());
    CHECK("" == instance.host());
    CHECK("/path/to/helicon/" == instance.pathname());
  }

  SECTION("test_host_non_special_scheme") {
    using namespace std::string_view_literals;

    auto instance = skyr::url{"sc://x/"};

    auto ec = instance.set_host(U"\x0000"sv);
    CHECK(ec);
    CHECK("sc://x/" == instance.href());
    CHECK("x" == instance.host());
    CHECK("x" == instance.hostname());
  }

  SECTION("test_host_non_special_scheme_1") {
    auto instance = skyr::url{"sc://test@test/"};

    auto ec = instance.set_host("");
    CHECK_FALSE(ec);
    CHECK("sc://test@test/" == instance.href());
    CHECK("test" == instance.username());
    CHECK("test" == instance.host());
    CHECK("test" == instance.hostname());
  }

  SECTION("test_host_port_overflow") {
    auto instance = skyr::url{"http://example.net/path"};

    auto ec = instance.set_host("example.com:65536");
    CHECK_FALSE(ec);
    CHECK("http://example.com/path" == instance.href());
    CHECK("example.com" == instance.host());
    CHECK("example.com" == instance.hostname());
    CHECK("" == instance.port());
  }

  SECTION("test_hostname_http") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_hostname("elpmaxe.com");
    CHECK_FALSE(ec);
    CHECK("http://elpmaxe.com/" == instance.href());
    CHECK("elpmaxe.com" == instance.hostname());
  }

  SECTION("test_hostname_with_port_number") {
    auto instance = skyr::url{"http://example.com:8080/"};

    auto ec = instance.set_hostname("elpmaxe.com");
    CHECK_FALSE(ec);
    CHECK("http://elpmaxe.com:8080/" == instance.href());
    CHECK("elpmaxe.com:8080" == instance.host());
    CHECK("elpmaxe.com" == instance.hostname());
  }

  SECTION("test_hostname_file_set_non_empty_host") {
    auto instance = skyr::url{"file:///path/to/helicon/"};

    auto ec = instance.set_hostname("example.com");
    CHECK_FALSE(ec);
    CHECK("file://example.com/path/to/helicon/" == instance.href());
    CHECK("example.com" == instance.hostname());
    CHECK("/path/to/helicon/" == instance.pathname());
  }

  SECTION("test_hostname_file_set_empty_host") {
    auto instance = skyr::url{"file://example.com/path/to/helicon/"};

    auto ec = instance.set_hostname("");
    CHECK_FALSE(ec);
    CHECK("file:///path/to/helicon/" == instance.href());
    CHECK("" == instance.hostname());
    CHECK("/path/to/helicon/" == instance.pathname());
  }

  SECTION("test_port_no_port") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_port("8080");
    CHECK_FALSE(ec);
    CHECK("http://example.com:8080/" == instance.href());
  }

  SECTION("test_port_existing_port") {
    auto instance = skyr::url{"http://example.com:80/"};

    auto ec = instance.set_port("8080");
    CHECK_FALSE(ec);
    CHECK("http://example.com:8080/" == instance.href());
  }

  SECTION("test_port_existing_port_no_port_1") {
    auto instance = skyr::url{"http://example.com:80/"};

    auto ec = instance.set_port("");
    CHECK_FALSE(ec);
    CHECK("http://example.com/" == instance.href());
  }

  SECTION("test_port_invalid_port_1") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_port("Ceci n'est pas un port");
    CHECK_FALSE(ec);
    CHECK("http://example.com/" == instance.href());
  }

  SECTION("test_port_invalid_port_2") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_port("1234567890");
    CHECK(ec);
    CHECK("http://example.com/" == instance.href());
  }

  SECTION("test_port_invalid_port_3") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_port("8080C");
    CHECK_FALSE(ec);
    CHECK("http://example.com:8080/" == instance.href());
  }

  SECTION("test_port_invalid_port_4") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_port("-1");
    CHECK_FALSE(ec);
    CHECK("http://example.com/" == instance.href());
  }

  SECTION("test_port_existing_port_no_port_2") {
    auto instance = skyr::url{"http://example.com:/"};

    auto ec = instance.set_port("");
    CHECK_FALSE(ec);
    CHECK("http://example.com/" == instance.href());
  }

  SECTION("test_port_no_port_int") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_port(8080);
    CHECK_FALSE(ec);
    CHECK("http://example.com:8080/" == instance.href());
  }

  SECTION("test_port_with_extra_characters") {
    auto instance = skyr::url{"http://example.com/path"};

    auto ec = instance.set_port("8080/stuff");
    CHECK_FALSE(ec);
    CHECK("http://example.com:8080/path" == instance.href());
  }

  SECTION("test_pathname_1") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_pathname("/path/to/helicon/");
    CHECK_FALSE(ec);
    CHECK("http://example.com/path/to/helicon/" == instance.href());
  }

  SECTION("test_pathname_2") {
    auto instance = skyr::url{"http://example.com/path/to/helicon/"};

    auto ec = instance.set_pathname("");
    CHECK_FALSE(ec);
    CHECK("http://example.com/" == instance.href());
  }

  SECTION("test_pathname_3") {
    auto instance = skyr::url{"file:///path/to/helicon/"};

    auto ec = instance.set_pathname("");
    CHECK_FALSE(ec);
    CHECK("file:///" == instance.href());
  }

  SECTION("test_pathname_4") {
    using namespace std::string_view_literals;

    auto instance = skyr::url{"a:/"};
    auto ec = instance.set_pathname(
        U"\x0000\x0001\t\n\r\x001f !\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\x007f\x0080\x0081\x00c9\x00e9"sv);
    CHECK_FALSE(ec);
    CHECK("a:/%00%01%1F%20!%22%23$%&'()*+,-./09:;%3C=%3E%3F@AZ[\\]^_%60az%7B|%7D~%7F%C2%80%C2%81%C3%89%C3%A9" == instance.href());
    CHECK("/%00%01%1F%20!%22%23$%&'()*+,-./09:;%3C=%3E%3F@AZ[\\]^_%60az%7B|%7D~%7F%C2%80%C2%81%C3%89%C3%A9" == instance.pathname());
  }

  SECTION("test_search_1") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_search("?a=b&c=d");
    CHECK_FALSE(ec);
    CHECK("http://example.com/?a=b&c=d" == instance.href());
  }

  SECTION("test_search_2") {
    auto instance = skyr::url{"http://example.com/"};

    auto ec = instance.set_search("a=b&c=d");
    CHECK_FALSE(ec);
    CHECK("http://example.com/?a=b&c=d" == instance.href());
  }

  SECTION("test_search_3") {
    auto instance = skyr::url{"http://example.com/#fragment"};

    auto ec = instance.set_search("?a=b&c=d");
    CHECK_FALSE(ec);
    CHECK("http://example.com/?a=b&c=d#fragment" == instance.href());
  }

  SECTION("test_search_4") {
    using namespace std::string_view_literals;

    auto instance = skyr::url{"a:/"};
    auto ec = instance.set_search(
        U"\x0000\x0001\t\n\r\x001f !\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\x007f\x0080\x0081\x00c9\x00e9"sv);
    CHECK_FALSE(ec);
    CHECK("a:/?%00%01%1F%20!%22%23$%&'()*+,-./09:;%3C=%3E?@AZ[\\]^_`az{|}~%7F%C2%80%C2%81%C3%89%C3%A9" == instance.href());
    CHECK("?%00%01%1F%20!%22%23$%&'()*+,-./09:;%3C=%3E?@AZ[\\]^_`az{|}~%7F%C2%80%C2%81%C3%89%C3%A9" == instance.search());
  }

  SECTION("test_hash_1") {
    auto instance = skyr::url{"http://example.com/"};
    auto ec = instance.set_hash("#fragment");
    CHECK_FALSE(ec);
    CHECK("http://example.com/#fragment" == instance.href());
  }

  SECTION("test_hash_2") {
    auto instance = skyr::url{"http://example.com/"};
    auto ec = instance.set_hash("fragment");
    CHECK_FALSE(ec);
    CHECK("http://example.com/#fragment" == instance.href());
  }

  SECTION("test_hash_3") {
    using namespace std::string_view_literals;

    auto instance = skyr::url{"a:/"};
    auto ec = instance.set_hash(
        U"\x0000\x0001\t\n\r\x001f !\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\x007f\x0080\x0081\x00c9\x00e9"sv);
    CHECK_FALSE(ec);
    CHECK("a:/#%00%01%1F%20!%22#$%&'()*+,-./09:;%3C=%3E?@AZ[\\]^_%60az{|}~%7F%C2%80%C2%81%C3%89%C3%A9" == instance.href());
    CHECK("#%00%01%1F%20!%22#$%&'()*+,-./09:;%3C=%3E?@AZ[\\]^_%60az{|}~%7F%C2%80%C2%81%C3%89%C3%A9" == instance.hash());
  }
}
