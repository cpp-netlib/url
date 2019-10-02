// Copyright 2017-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <fstream>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/url.hpp>
#include <nlohmann/json.hpp>

// Tests using test data from W3C
// https://github.com/w3c/web-platform-tests/tree/master/url

using json = nlohmann::json;

namespace {
struct test_case {
  test_case(json object) {
    input = object["input"].get<std::string>();
    base = object["base"].get<std::string>();

    failure =
        (object.find("failure") != object.end()) &&
        object["failure"].get<bool>();
    if (!failure) {
      href = object["href"].get<std::string>();
      if (object.find("origin") != object.end()) {
        origin = object["origin"].get<std::string>();
      }
      protocol = object["protocol"].get<std::string>();
      username = object["username"].get<std::string>();
      password = object["password"].get<std::string>();
      host = object["host"].get<std::string>();
      hostname = object["hostname"].get<std::string>();
      port = object["port"].get<std::string>();
      pathname = object["pathname"].get<std::string>();
      search = object["search"].get<std::string>();
      hash = object["hash"].get<std::string>();
    }
  }

  std::string input;
  std::string base;
  bool failure;
  std::string href;
  std::string origin;
  std::string protocol;
  std::string username;
  std::string password;
  std::string host;
  std::string hostname;
  std::string port;
  std::string pathname;
  std::string search;
  std::string hash;
};

class TestCaseGenerator : public Catch::Generators::IGenerator<test_case> {
 public:
  explicit TestCaseGenerator(
      const std::string &filename, bool failure) {
    std::ifstream fs{filename};
    json tests;
    fs >> tests;

    for (auto &&test_case_object : tests) {
      if (!test_case_object.is_string()) {
      auto test_case_data = test_case{test_case_object};
        if (failure == test_case_data.failure) {
          test_case_data_.emplace_back(test_case_data);
        }
      }
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
    const std::string &filename, bool failure) {
  return Catch::Generators::GeneratorWrapper<test_case>(
      std::unique_ptr<Catch::Generators::IGenerator<test_case>>(
          new TestCaseGenerator(filename, failure)));
}
} // namespace


TEST_CASE("test_parse_urls_using_base_urls", "[web_platorm]") {
  auto test_case_data = GENERATE(test_case_("urltestdata.json", false));

  SECTION("parse_using_constructor") {
    auto instance = skyr::url(
        test_case_data.input,
        skyr::url(test_case_data.base));
    CHECK(test_case_data.href == instance.href());
    CHECK(test_case_data.protocol == instance.protocol());
    CHECK(test_case_data.username == instance.username());
    CHECK(test_case_data.password == instance.password());
    CHECK(test_case_data.host == instance.host());
    CHECK(test_case_data.hostname == instance.hostname());
    CHECK(test_case_data.port == instance.port());
    CHECK(test_case_data.pathname == instance.pathname());
    CHECK(test_case_data.search == instance.search());
    CHECK(test_case_data.hash == instance.hash());
  }
}

TEST_CASE("test_parse_urls_using_base_urls_failures", "[web_platform]") {
  auto test_case_data = GENERATE(test_case_("urltestdata.json", true));

  SECTION("parse_using_constructor") {
    auto base = skyr::url(test_case_data.base);
    REQUIRE_THROWS_AS(skyr::url(test_case_data.input, base), skyr::url_parse_error);
  }
}
