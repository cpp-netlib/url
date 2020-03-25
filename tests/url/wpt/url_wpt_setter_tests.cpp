// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>
#include <skyr/url.hpp>

// Tests using test data from W3C
// https://github.com/w3c/web-platform-tests/tree/master/url

using json = nlohmann::json;

namespace {
struct test_case {
  test_case(const std::string &part, json object) {
    this->part = part;
    href = object["href"].get<std::string>();
    new_value = object["new_value"].get<std::string>();
    auto expected_object = object["expected"];
    expected = {
        expected_object["href"].get<std::string>(), expected_object[part].get<std::string>() };
  }

  std::string part;
  std::string href;
  std::string new_value;
  std::pair<std::string, std::string> expected;

};

class TestCaseGenerator : public Catch::Generators::IGenerator<test_case> {
 public:
  explicit TestCaseGenerator(
      const std::string &filename,
      const std::string &part) {
    std::ifstream fs{filename};
    json tests;
    fs >> tests;

    for (auto &&test_case_object : tests[part]) {
      test_case_data_.emplace_back(part, test_case_object);
    }
    it_ = begin(test_case_data_);
  }

  const test_case &get() const override {
    assert(it_ != test_case_data_.end());
    return *it_;
  }

  bool next() override {
    assert(it_ != test_case_data_.end());
    ++it_;
    return it_ != test_case_data_.end();
  }

 private:
  std::vector<test_case> test_case_data_;
  std::vector<test_case>::const_iterator it_;
};

Catch::Generators::GeneratorWrapper<test_case> test_case_(
    const std::string &part, const std::string &filename) {
  return Catch::Generators::GeneratorWrapper<test_case>(
      std::unique_ptr<Catch::Generators::IGenerator<test_case>>(
          new TestCaseGenerator(filename, part)));
}
} // namespace



TEST_CASE("protocol", "[web_platform]") {
  auto test_case_data = GENERATE(test_case_("protocol", "setters_tests.json"));

  SECTION("set protocol") {
    INFO(test_case_data.href << ", " << test_case_data.new_value);
    auto instance = skyr::make_url(test_case_data.href);
    REQUIRE(instance);
    instance.value().set_protocol(test_case_data.new_value);
    CHECK(test_case_data.expected.first == instance.value().href());
    CHECK(test_case_data.expected.second == instance.value().protocol());
  }
}

TEST_CASE("username", "[web_platform]") {
  auto test_case_data = GENERATE(test_case_("username", "setters_tests.json"));

  SECTION("set username") {
    INFO(test_case_data.href << ", " << test_case_data.new_value);
    auto instance = skyr::make_url(test_case_data.href);
    REQUIRE(instance);
    instance.value().set_username(test_case_data.new_value);
    CHECK(test_case_data.expected.first == instance.value().href());
    CHECK(test_case_data.expected.second == instance.value().username());
  }
}

TEST_CASE("password", "[web_platform]") {
  auto test_case_data = GENERATE(test_case_("password", "setters_tests.json"));

  SECTION("set password") {
    INFO(test_case_data.href << ", " << test_case_data.new_value);
    auto instance = skyr::make_url(test_case_data.href);
    REQUIRE(instance);
    instance.value().set_password(test_case_data.new_value);
    CHECK(test_case_data.expected.first == instance.value().href());
    CHECK(test_case_data.expected.second == instance.value().password());
  }
}

TEST_CASE("host", "[web_platform][!mayfail]") {
  auto test_case_data = GENERATE(test_case_("host", "setters_tests.json"));

  SECTION("set host") {
    INFO(test_case_data.href << ", " << test_case_data.new_value);
    auto instance = skyr::make_url(test_case_data.href);
    REQUIRE(instance);
    instance.value().set_host(test_case_data.new_value);
    CHECK(test_case_data.expected.first == instance.value().href());
    CHECK(test_case_data.expected.second == instance.value().host());
  }
}

TEST_CASE("port", "[web_platform]") {
  auto test_case_data = GENERATE(test_case_("port", "setters_tests.json"));

  SECTION("set port") {
    INFO(test_case_data.href << ", " << test_case_data.new_value);
    auto instance = skyr::make_url(test_case_data.href);
    REQUIRE(instance);
    instance.value().set_port(test_case_data.new_value);
    CHECK(test_case_data.expected.first == instance.value().href());
    CHECK(test_case_data.expected.second == instance.value().port());
  }
}

TEST_CASE("pathname", "[web_platform][!mayfail]") {
  auto test_case_data = GENERATE(test_case_("pathname", "setters_tests.json"));

  SECTION("set pathname") {
    INFO(test_case_data.href << ", " << test_case_data.new_value);
    auto instance = skyr::make_url(test_case_data.href);
    REQUIRE(instance);
    instance.value().set_pathname(test_case_data.new_value);
    CHECK(test_case_data.expected.first == instance.value().href());
    CHECK(test_case_data.expected.second == instance.value().pathname());
  }
}

TEST_CASE("search", "[web_platform][!mayfail]") {
  auto test_case_data = GENERATE(test_case_("search", "setters_tests.json"));

  SECTION("set search") {
    INFO(test_case_data.href << ", " << test_case_data.new_value);
    auto instance = skyr::make_url(test_case_data.href);
    REQUIRE(instance);
    instance.value().set_search(test_case_data.new_value);
    CHECK(test_case_data.expected.first == instance.value().href());
    CHECK(test_case_data.expected.second == instance.value().search());
  }
}

TEST_CASE("hash", "[web_platform][!mayfail]") {
  auto test_case_data = GENERATE(test_case_("hash", "setters_tests.json"));

  SECTION("set hash") {
    INFO(test_case_data.href << ", " << test_case_data.new_value);
    auto instance = skyr::make_url(test_case_data.href);
    REQUIRE(instance);
    instance.value().set_hash(test_case_data.new_value);
    CHECK(test_case_data.expected.first == instance.value().href());
    CHECK(test_case_data.expected.second == instance.value().hash());
  }
}
