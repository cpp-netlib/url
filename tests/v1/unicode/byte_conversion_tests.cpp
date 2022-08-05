// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <catch2/catch_all.hpp>
#include <skyr/v1/unicode/details/to_u8.hpp>

TEST_CASE("weird_01", "byte_conversion_tests") {
  auto bytes = skyr::details::to_u8(U"http://\xfdD0zyx.com");
  CHECK(bytes);
}

TEST_CASE("weird_02", "byte_conversion_tests") {
  auto bytes = skyr::details::to_u8("/\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
  CHECK(bytes);
}
