// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_all.hpp>
#include <skyr/v1/network/ipv6_address.hpp>

TEST_CASE("ipv6_address_tests", "[ipv6]") {
  using namespace std::string_literals;

  SECTION("zero_test") {
    auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 0}};
    auto instance = skyr::ipv6_address(address);
    CHECK("::" == instance.serialize());
  }

  SECTION("loopback_test") {
    auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 1}};
    auto instance = skyr::ipv6_address(address);
    CHECK("::1" == instance.serialize());
  }

  SECTION("ipv6_address_test_1") {
    const auto address = "1080:0:0:0:8:800:200C:417A"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("1080::8:800:200c:417a" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_2") {
    const auto address = "2001:db8:85a3:8d3:1319:8a2e:370:7348"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("2001:db8:85a3:8d3:1319:8a2e:370:7348" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_3") {
    const auto address = "2001:db8:85a3:0:0:8a2e:370:7334"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("2001:db8:85a3::8a2e:370:7334" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_4") {
    const auto address = "2001:db8:85a3::8a2e:370:7334"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("2001:db8:85a3::8a2e:370:7334" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_5") {
    const auto address = "2001:0db8:0000:0000:0000:0000:1428:57ab"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_6") {
    const auto address = "2001:0db8:0000:0000:0000::1428:57ab"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_7") {
    const auto address = "2001:0db8:0:0:0:0:1428:57ab"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_8") {
    const auto address = "2001:0db8:0:0::1428:57ab"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_9") {
    const auto address = "2001:0db8::1428:57ab"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_10") {
    const auto address = "2001:db8::1428:57ab"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("2001:db8::1428:57ab" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_11") {
    const auto address = "::ffff:0c22:384e"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("::ffff:c22:384e" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_12") {
    const auto address = "fe80::"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("fe80::" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_address_test_13") {
    const auto address = "::ffff:c000:280"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("::ffff:c000:280" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_loopback_test_1") {
    const auto address = "0000:0000:0000:0000:0000:0000:0000:0001"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("::1" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_v4inv6_test_1") {
    const auto address = "::ffff:12.34.56.78"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("::ffff:c22:384e" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("ipv6_v4inv6_test_2") {
    const auto address = "::ffff:192.0.2.128"s;
    bool validation_error = false;
    auto instance = skyr::parse_ipv6_address(address, &validation_error);
    REQUIRE(instance);
    CHECK("::ffff:c000:280" == instance.value().serialize());
    CHECK(!validation_error);
  }

  SECTION("loopback_test") {
    auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 1}};
    auto instance = skyr::ipv6_address(address);
    std::array<unsigned char, 16> bytes{
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      }};
    CHECK(bytes == instance.to_bytes());
  }
}
