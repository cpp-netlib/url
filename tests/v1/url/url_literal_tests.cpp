// Copyright 2019-20 Glyn Matthews
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <memory>
#include <catch2/catch_all.hpp>
#include <skyr/v1/url.hpp>

using namespace skyr::literals;

TEST_CASE("url_tests", "[url]") {
  SECTION("construct url from literal") {
    CHECK_NOTHROW("http://www.example.com/"_url);
  }

  SECTION("construct url from wchar_t literal") {
    CHECK_NOTHROW(L"http://www.example.com/"_url);
  }

  SECTION("construct url from char16_t literal") {
    CHECK_NOTHROW(u"http://www.example.com/"_url);
  }

  SECTION("construct url from char32_t literal") {
    CHECK_NOTHROW(U"http://www.example.com/"_url);
  }
}
