// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../../src/url/ipv4_address.hpp"

TEST_CASE("ipv4 addresses", "[ipv4]") {
  SECTION("zero_test") {
    auto instance = skyr::ipv4_address(0);
    CHECK("0.0.0.0" == instance.to_string());
  }

  SECTION("loopback_test") {
    auto instance = skyr::ipv4_address(0x7f000001);
    CHECK("127.0.0.1" == instance.to_string());
  }

  SECTION("address_test") {
    auto instance = skyr::ipv4_address(0x814ff5fc);
    CHECK("129.79.245.252" == instance.to_string());
  }

  SECTION("parse_zero_test") {
    auto address = std::string("0.0.0.0");
    auto instance = skyr::parse_ipv4_address(address);
    REQUIRE(instance);
    CHECK(0 == instance.value().address());
  }

  SECTION("parse_loopback_test") {
    auto address = std::string("127.0.0.1");
    auto instance = skyr::parse_ipv4_address(address);
    REQUIRE(instance);
    CHECK(0x7f000001 == instance.value().address());
  }

  SECTION("parse_address_test") {
    auto address = std::string("129.79.245.252");
    auto instance = skyr::parse_ipv4_address(address);
    REQUIRE(instance);
    CHECK(0x814ff5fc == instance.value().address());
  }

  SECTION("parse_address_with_hex") {
    auto address = std::string("0x7f.0.0.0x7f");
    auto instance = skyr::parse_ipv4_address(address);
    REQUIRE(instance);
    CHECK(0x7f00007f == instance.value().address());
  }

  SECTION("parse_invalid_address_with_hex") {
    auto address = std::string("0x7f.0.0.0x7g");
    auto instance = skyr::parse_ipv4_address(address);
    REQUIRE_FALSE(instance);
  }
}
