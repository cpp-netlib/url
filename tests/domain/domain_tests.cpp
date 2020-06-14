// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <skyr/domain/domain.hpp>
#include <skyr/domain/errors.hpp>

TEST_CASE("valid domains to ascii", "[domain]") {
  using param = std::pair<std::string, std::string>;

  auto domain = GENERATE(
      param{"example.com", "example.com"},
      param{"⌘.ws", "xn--bih.ws"},
      param{"你好你好", "xn--6qqa088eba"},
      param{"你好你好.com", "xn--6qqa088eba.com"},
      param{"उदाहरण.परीक्षा", "xn--p1b6ci4b4b3a.xn--11b5bs3a9aj6g"},
      param{"faß.ExAmPlE", "xn--fa-hia.example"},
      param{"βόλος.com", "xn--nxasmm1c.com"},
      param{"Ｇｏ.com", "go.com"});

  SECTION("domain_to_ascii_tests") {
    const auto &[input, expected] = domain;
    auto instance = skyr::domain_to_ascii(input);
    INFO("input = " << input << ", expected = " << expected);
    REQUIRE(instance);
    CHECK(expected == instance.value());
  }
}

TEST_CASE("valid domains from ascii", "[domain]") {
  using param = std::pair<std::string, std::string>;

  auto domain = GENERATE(
      param{"example.com", "example.com"},
      param{"⌘.ws", "xn--bih.ws"},
      param{"你好你好", "xn--6qqa088eba"},
      param{"你好你好.com", "xn--6qqa088eba.com"},
      param{"उदाहरण.परीक्षा", "xn--p1b6ci4b4b3a.xn--11b5bs3a9aj6g"},
      param{"βόλος.com", "xn--nxasmm1c.com"});

  SECTION("ascii_to_domain_tests") {
    const auto &[expected, input] = domain;
    auto instance = skyr::domain_to_u8(input);
    REQUIRE(instance);
    CHECK(expected == instance.value());
  }
}

TEST_CASE("invalid domains", "[domain]") {
  SECTION("invalid_domain_1") {
    auto instance = skyr::domain_to_ascii("GOO\xc2\xa0\xE3\x80\x80goo.com");
    REQUIRE(instance);
  }

  SECTION("invalid_domain_1_be strict") {
    auto instance = skyr::domain_to_ascii("GOO\xc2\xa0\xE3\x80\x80goo.com", true);
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_domain_2") {
    auto instance = skyr::domain_to_ascii("\xef\xbf\xbd");
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_domain_3") {
    auto instance = skyr::domain_to_ascii("％\xef\xbc\x94\xef\xbc\x91.com");
    REQUIRE(instance);
  }

  SECTION("invalid_domain_4_bestrict") {
    auto instance = skyr::domain_to_ascii("％\xef\xbc\x94\xef\xbc\x91.com", true);
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_name_long_name") {
    auto domain = std::string(300, 'x') + ".com";
    auto instance = skyr::domain_to_ascii(domain, true);
    REQUIRE_FALSE(instance);
    REQUIRE(instance.error() == skyr::domain_errc::invalid_length);
  }
}

TEST_CASE("web platform tests", "[domain][!mayfail]") {
  /// Invalid Punycode
  SECTION("toascii_01") {
    auto instance = skyr::domain_to_ascii("xn--a");
    REQUIRE_FALSE(instance);
  }

  SECTION("toascii_02") {
    auto instance = skyr::domain_to_ascii("xn--a.xn--nxa");
    REQUIRE_FALSE(instance);
  }

  SECTION("toascii_03") {
    auto instance = skyr::domain_to_ascii("xn--a.β");
    REQUIRE_FALSE(instance);
  }

  /// CheckJoiners
  SECTION("toascii_04") {
    auto instance = skyr::domain_to_ascii("\xe2\x80\x8d.example");
    REQUIRE_FALSE(instance);
  }

  SECTION("toascii_05") {
    auto instance = skyr::domain_to_ascii("xn--1ug.example");
    REQUIRE_FALSE(instance);
  }

  /// CheckBidi
  SECTION("toascii_06") {
    auto instance = skyr::domain_to_ascii("a\xd9\x8a");
    REQUIRE_FALSE(instance);
  }

  SECTION("toascii_07") {
    auto instance = skyr::domain_to_ascii("xn--a-yoc");
    REQUIRE_FALSE(instance);
  }

  /// ProcessingOptions is non-transtional
  SECTION("toascii_08") {
    auto instance = skyr::domain_to_ascii("ශ්‍රී");
    REQUIRE(instance);
    CHECK("xn--10cl1a0b660p" == instance.value());
  }

  SECTION("toascii_09") {
    auto instance = skyr::domain_to_ascii("نامه‌ای");
    REQUIRE(instance);
    CHECK("xn--mgba3gch31f060k" == instance.value());
  }

  /// U+FFFD character encoded in Punycode
  SECTION("toascii_10") {
    auto instance = skyr::domain_to_ascii("xn--zn7c.com");
    REQUIRE_FALSE(instance);
  }
}
