// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <string>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <skyr/v1/containers/static_vector.hpp>

struct test_destructor_call {
  bool *destructed;

  test_destructor_call(bool *destructed)
  : destructed(destructed) {
    *destructed = false;
  }

  ~test_destructor_call() {
    *destructed = true;
  }

};

TEST_CASE("pop back calls destructor", "[containers]") {
  auto destructed = false;

  auto vector = skyr::static_vector<std::unique_ptr<test_destructor_call>, 8>{};
  vector.emplace_back(std::make_unique<test_destructor_call>(&destructed));
  CHECK_FALSE(destructed);
  vector.pop_back();
  CHECK(destructed);
}

TEST_CASE("clear calls destructor", "[containers]") {
  auto destructed = false;

  auto vector = skyr::static_vector<std::unique_ptr<test_destructor_call>, 8>{};
  vector.emplace_back(std::make_unique<test_destructor_call>(&destructed));
  CHECK_FALSE(destructed);
  vector.clear();
  CHECK(destructed);
}
