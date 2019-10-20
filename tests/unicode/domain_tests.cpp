// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/unicode/domain.hpp>

TEST_CASE("valid domains", "[domain]") {
  using param = std::pair<std::string, std::string>;

  auto domain = GENERATE(
      param{u8"example.com", "example.com"}, param{"⌘.ws", "xn--bih.ws"},
      param{u8"你好你好", "xn--6qqa088eba"},
      param{u8"你好你好.com", "xn--6qqa088eba.com"},
      param{u8"उदाहरण.परीक्षा", "xn--p1b6ci4b4b3a.xn--11b5bs3a9aj6g"},
      param{u8"faß.ExAmPlE", "xn--fa-hia.example"},
      param{u8"βόλος.com", "xn--nxasmm1c.com"},
      param{u8"Ｇｏ.com", "go.com"});

  SECTION("dom ascii_tests") {
    auto [input, expected] = domain;
    auto instance = skyr::unicode::domain_to_ascii(input);
    REQUIRE(instance);
    CHECK(expected == instance.value());
  }
}

TEST_CASE("invalid domains", "[domain]") {
  SECTION("invalid_domain_1") {
    auto instance = skyr::unicode::domain_to_ascii("GOO 　goo.com");
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_domain_2") {
    auto instance = skyr::unicode::domain_to_ascii(U"\xfdD0zyx.com");
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_domain_3") {
    auto instance = skyr::unicode::domain_to_ascii("�");
    REQUIRE_FALSE(instance);
  }

  SECTION("invalid_domain_4") {
    auto instance = skyr::unicode::domain_to_ascii("％４１.com");
    REQUIRE_FALSE(instance);
  }
}
