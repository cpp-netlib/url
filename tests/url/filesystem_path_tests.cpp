// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/url.hpp>
#include <skyr/url/filesystem.hpp>

TEST_CASE("filesystem path", "[filesystem_path]") {
  SECTION("empty_path") {
    auto instance = skyr::url{};
    auto path = skyr::filesystem::to_path(instance);
    REQUIRE(path);
    CHECK(path.value().empty());
  }

  SECTION("file_path") {
    auto instance = skyr::url{"file:///path/to/file.txt"};
    auto path = skyr::filesystem::to_path(instance);
    REQUIRE(path);
    CHECK(path.value().generic_string() == "/path/to/file.txt");
  }

  SECTION(filesystem_path_tests, http_path) {
    auto instance = skyr::url{"http://www.example.com/path/to/file.txt"};
    auto path = skyr::filesystem::to_path(instance);
    REQUIRE(path);
    CHECK(path.value().generic_string() == "/path/to/file.txt");
  }

  // TODO: add test with percent encoding
  // TODO: add test with unicode
}
