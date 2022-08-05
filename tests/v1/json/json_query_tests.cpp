// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <catch2/catch_all.hpp>
#include <vector>
#include <skyr/v1/json/json.hpp>

TEST_CASE("ipv6_address_tests", "[json.query]") {
  using namespace std::string_literals;

  SECTION("decode_simple_query") {
    auto query = "?a=b&c=d"s;
    auto json = skyr::json::decode_query(query);
    CHECK(json["a"] == "b");
    CHECK(json["c"] == "d");
  }

  SECTION("decode_simple_query_with_multiple_values") {
    auto query = "?a=b&c=d&a=e"s;
    auto json = skyr::json::decode_query(query);
    CHECK(json["a"].get<std::vector<std::string>>() == std::vector<std::string>{"b", "e"});
    CHECK(json["c"] == "d");
  }

  SECTION("decode_simple_query_with_unicode_value") {
    auto query = "?a=%CF%80"s;
    auto json = skyr::json::decode_query(query);
    CHECK(json["a"] == "\xcf\x80");
  }

//  SECTION("decode_empty_query") {
//    auto query = ""s;
//    auto json = skyr::json::decode_query(query);
//    CHECK(json.empty());
//  }

  SECTION("encode_simple_query") {
    auto json = nlohmann::json{ { "a", "b" }, { "c", "d" } };
    auto query = skyr::json::encode_query(json);
    REQUIRE(query);
    CHECK(query.value() == "a=b&c=d");
  }

  SECTION("encode_simple_query_with_multiple_arguments") {
    auto json = nlohmann::json{ { "a", { "b", "e" } }, { "c", "d" } };
    auto query = skyr::json::encode_query(json);
    REQUIRE(query);
    CHECK(query.value() == "a=b&a=e&c=d");
  }

  SECTION("encode_simple_query_with_unicode_value") {
    auto json = nlohmann::json{ { "a", "\xcf\x80" } };
    auto query = skyr::json::encode_query(json);
    REQUIRE(query);
    CHECK(query.value() == "a=%CF%80");
  }

  SECTION("invalid_query_json") {
    auto json = nlohmann::json{
      {"I", "am", "not", "a", "valid", "query", "string"},
    };
    auto query = skyr::json::encode_query(json);
    REQUIRE_FALSE(query);
  }
}
