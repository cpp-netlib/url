// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <catch2/catch_all.hpp>
#include <skyr/v2/url.hpp>

TEST_CASE("url_vector_tests", "[url]") {
  SECTION("vector") {
    std::vector<skyr::url> urls{
        skyr::url("http://www.example.com/"),
        skyr::url("http://www.example.org/")
    };

    CHECK(urls.size() == 2);
  }

  SECTION("sorted_vector") {
    std::vector<skyr::url> urls{
        skyr::url("http://www.example.org/"),
        skyr::url("http://www.example.com/")
    };
    std::sort(std::begin(urls), std::end(urls));

    REQUIRE(urls.size() == 2);
    CHECK(static_cast<std::string>(urls[0]) == "http://www.example.com/");
    CHECK(static_cast<std::string>(urls[1]) == "http://www.example.org/");
  }
}
