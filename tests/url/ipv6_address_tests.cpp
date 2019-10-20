// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../../src/url/ipv6_address.hpp"

TEST_CASE("ipv6_address_tests", "[ipv6]") {
  SECTION("zero_test") {
    auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 0}};
    auto instance = skyr::ipv6_address(address);
    CHECK("::" == instance.to_string());
  }

  SECTION("loopback_test") {
    auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 1}};
    auto instance = skyr::ipv6_address(address);
    CHECK("::1" == instance.to_string());
  }

  SECTION("ipv6_address_test_1") {
    auto address = std::string("1080:0:0:0:8:800:200C:417A");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("1080::8:800:200c:417a" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_2") {
    auto address = std::string("2001:db8:85a3:8d3:1319:8a2e:370:7348");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("2001:db8:85a3:8d3:1319:8a2e:370:7348" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_3") {
    auto address = std::string("2001:db8:85a3:0:0:8a2e:370:7334");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("2001:db8:85a3::8a2e:370:7334" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_4") {
    auto address = std::string("2001:db8:85a3::8a2e:370:7334");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("2001:db8:85a3::8a2e:370:7334" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_5") {
    auto address = std::string("2001:0db8:0000:0000:0000:0000:1428:57ab");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_6") {
    auto address = std::string("2001:0db8:0000:0000:0000::1428:57ab");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_7") {
    auto address = std::string("2001:0db8:0:0:0:0:1428:57ab");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_8") {
    auto address = std::string("2001:0db8:0:0::1428:57ab");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_9") {
    auto address = std::string("2001:0db8::1428:57ab");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_10") {
    auto address = std::string("2001:db8::1428:57ab");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_11") {
    auto address = std::string("::ffff:0c22:384e");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("::ffff:c22:384e" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_12") {
    auto address = std::string("fe80::");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("fe80::" == instance.value().to_string());
  }

  SECTION("ipv6_address_test_13") {
    auto address = std::string("::ffff:c000:280");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("::ffff:c000:280" == instance.value().to_string());
  }

  SECTION("ipv6_loopback_test_1") {
    auto address = std::string("0000:0000:0000:0000:0000:0000:0000:0001");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("::1" == instance.value().to_string());
  }

  SECTION("ipv6_v4inv6_test_1") {
    auto address = std::string("::ffff:12.34.56.78");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("::ffff:c22:384e" == instance.value().to_string());
  }

  SECTION("ipv6_v4inv6_test_2") {
    auto address = std::string("::ffff:192.0.2.128");
    auto instance = skyr::parse_ipv6_address(address);
    REQUIRE(instance);
    CHECK("::ffff:c000:280" == instance.value().to_string());
  }
}
