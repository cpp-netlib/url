// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/domain/domain.hpp>
#include <skyr/domain/errors.hpp>

TEST_CASE("valid domains to ascii", "[domain]") {
  using param = std::pair<std::string, std::string>;

  auto domain = GENERATE(
      param{"example.com", "example.com"}, param{"⌘.ws", "xn--bih.ws"},
      param{"你好你好", "xn--6qqa088eba"},
      param{"你好你好.com", "xn--6qqa088eba.com"},
      param{"उदाहरण.परीक्षा", "xn--p1b6ci4b4b3a.xn--11b5bs3a9aj6g"},
      param{"faß.ExAmPlE", "xn--fa-hia.example"},
      param{"βόλος.com", "xn--nxasmm1c.com"},
      param{"Ｇｏ.com", "go.com"});

  SECTION("domain_to_ascii_tests") {
    const auto &[input, expected] = domain;
    auto instance = skyr::domain_to_ascii(input);
    REQUIRE(instance);
    CHECK(expected == instance.value());
  }
}

TEST_CASE("valid domains from ascii", "[domain]") {
  using param = std::pair<std::string, std::string>;

  auto domain = GENERATE(
      param{"example.com", "example.com"}, param{"⌘.ws", "xn--bih.ws"},
      param{"你好你好", "xn--6qqa088eba"},
      param{"你好你好.com", "xn--6qqa088eba.com"},
      param{"उदाहरण.परीक्षा", "xn--p1b6ci4b4b3a.xn--11b5bs3a9aj6g"},
      param{"βόλος.com", "xn--nxasmm1c.com"});

  SECTION("ascii_to_domain_tests") {
    const auto &[expected, input] = domain;
    auto instance = skyr::domain_to_unicode(input);
    REQUIRE(instance);
    CHECK(expected == instance.value());
  }
}

TEST_CASE("invalid domains", "[domain]") {
  SECTION("invalid_domain_1") {
    auto instance = skyr::domain_to_ascii("GOO 　goo.com");
    REQUIRE(instance);
  }

  SECTION("invalid_domain_1_be strict") {
    auto instance = skyr::domain_to_ascii("GOO 　goo.com", true);
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_domain_2") {
    auto instance = skyr::domain_to_ascii(U"\xfdD0zyx.com");
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_domain_3") {
    auto instance = skyr::domain_to_ascii("�");
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_domain_4") {
    auto instance = skyr::domain_to_ascii("％４１.com");
    REQUIRE(instance);
  }

  SECTION("invalid_domain_4_bestrict") {
    auto instance = skyr::domain_to_ascii("％４１.com", true);
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_name_long_name") {
    auto domain = std::string(300, 'x') + ".com";
    auto instance = skyr::domain_to_ascii(domain, true);
    REQUIRE_FALSE(instance);
    REQUIRE(static_cast<skyr::domain_errc>(instance.error().value()) == skyr::domain_errc::invalid_length);
  }
}
