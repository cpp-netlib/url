// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <catch2/catch_all.hpp>
#include <skyr/v1/domain/domain.hpp>
#include <skyr/v1/domain/errors.hpp>

TEST_CASE("valid domains to ascii", "[domain]") {
  using param = std::pair<std::string, std::string>;

  auto domain = GENERATE(
      param{"example.com", "example.com"},
      param{"sub.example.com", "sub.example.com"},
      param{"⌘.ws", "xn--bih.ws"},
      param{"你好你好", "xn--6qqa088eba"},
      param{"你好你好.com", "xn--6qqa088eba.com"},
      param{"उदाहरण.परीक्षा", "xn--p1b6ci4b4b3a.xn--11b5bs3a9aj6g"},
      param{"faß.ExAmPlE", "xn--fa-hia.example"},
      param{"βόλος.com", "xn--nxasmm1c.com"},
      param{"Ｇｏ.com", "go.com"});

  SECTION("domain_to_ascii_tests") {
    const auto &[input, expected] = domain;
    INFO("input = " << input << ", expected = " << expected);
    auto output = std::string{};
    auto result = skyr::domain_to_ascii(input, &output);
    REQUIRE(result);
    CHECK(expected == output);
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
    auto output = std::string{};
    auto result = skyr::domain_to_u8(input, &output);
    REQUIRE(result);
    CHECK(expected == output);
  }
}

TEST_CASE("invalid domains", "[domain]") {
  //  SECTION("invalid_domain_1") {
  //    auto instance = skyr::domain_to_ascii("GOO\xc2\xa0\xE3\x80\x80goo.com", false);
  //    REQUIRE(instance);
  //  }

  SECTION("invalid_domain_1_be strict") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("GOO\xc2\xa0\xE3\x80\x80goo.com", &output, true);
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_domain_2") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("\xef\xbf\xbd", &output);
    REQUIRE_FALSE(instance);
  }

  //  SECTION("invalid_domain_3") {
  //    auto instance = skyr::domain_to_ascii("％\xef\xbc\x94\xef\xbc\x91.com", false);
  //    REQUIRE(instance);
  //  }

  SECTION("invalid_domain_4_bestrict") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("％\xef\xbc\x94\xef\xbc\x91.com", &output, true);
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_name_long_name") {
    auto domain = std::string(300, 'x') + ".com";
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii(domain, &output, true);
    REQUIRE_FALSE(instance);
    REQUIRE(instance.error() == skyr::domain_errc::invalid_length);
  }
}

TEST_CASE("web platform tests", "[domain]") {
  /// Invalid Punycode
  SECTION("toascii_01") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("xn--a", &output);
    REQUIRE_FALSE(instance);
  }

  SECTION("toascii_02") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("xn--a.xn--nxa", &output);
    REQUIRE_FALSE(instance);
  }

  SECTION("toascii_03") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("xn--a.β", &output);
    REQUIRE_FALSE(instance);
  }

    /// ProcessingOptions is non-transitional
  SECTION("toascii_08") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("ශ්‍රී", &output);
    REQUIRE(instance);
    CHECK("xn--10cl1a0b660p" == output);
  }

  SECTION("toascii_09") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("نامه‌ای", &output);
    REQUIRE(instance);
    CHECK("xn--mgba3gch31f060k" == output);
  }

    /// U+FFFD (replacement character)
  SECTION("toascii_10") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("\xef\xbf\xbd.com", &output);
    REQUIRE_FALSE(instance);
  }

    /// U+FFFD character encoded in Punycode
  SECTION("toascii_11") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("xn--zn7c.com", &output);
    REQUIRE_FALSE(instance);
  }
}

TEST_CASE("web platform tests mayfail", "[domain][!mayfail]") {
  /// CheckJoiners
  SECTION("toascii_04") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("\xe2\x80\x8d.example", &output);
    REQUIRE_FALSE(instance);
  }

  SECTION("toascii_05") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("xn--1ug.example", &output);
    REQUIRE_FALSE(instance);
  }

  /// CheckBidi
  SECTION("toascii_06") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("a\xd9\x8a", &output);
    REQUIRE_FALSE(instance);
  }

  SECTION("toascii_07") {
    auto output = std::string{};
    auto instance = skyr::domain_to_ascii("xn--a-yoc", &output);
    REQUIRE_FALSE(instance);
  }
}
