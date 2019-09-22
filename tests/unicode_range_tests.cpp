// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <string_view>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <range/v3/empty.hpp>
#include <skyr/unicode/range/octet_range.hpp>
#include <skyr/unicode/range/u16_range.hpp>
#include <skyr/unicode/range/u32_range.hpp>


TEST_CASE("code point tests") {
  using std::begin;
  using std::end;

  SECTION("u8 code point 01") {
    auto bytes = std::string("\xf0\x9f\x92\xa9");
    auto cp = skyr::unicode::code_point_octets(bytes);
    REQUIRE(cp);
    CHECK(std::string("\xf0\x9f\x92\xa9") == std::string(begin(cp.value()), end(cp.value())));
    CHECK(U'\x1f4a9' == u32(cp.value()));
//    CHECK(u'\xd83d' == std::get<0>(u16(cp.value())));
//    CHECK(u'\xdca9' == std::get<1>(u16(cp.value())));
  }

  SECTION("u8 code point 02") {
    auto bytes = std::string("\x9f\x92\xa9");
    auto cp = skyr::unicode::code_point_octets(bytes);
    REQUIRE(!cp);
  }
}

TEST_CASE("octet range iterator") {
  using iterator_type = skyr::unicode::octet_range_iterator<std::string::const_iterator>;

  SECTION("construction") {
    auto bytes = std::string("\xf0\x9f\x92\xa9");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F4A9' == u32(code_point.value()));
  }

  SECTION("increment") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F3F3' == u32(code_point.value()));
    ++it;
    code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\xFE0F' == u32(code_point.value()));
  }

  SECTION("increment invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    CHECK(!*it);
  }

  SECTION("equality") {
    auto bytes = std::string("\xf0\x9f\x92\xa9");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto last = iterator_type();
    ++it;
    CHECK(last == it);
  }

  SECTION("inequality") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto last = iterator_type();
    CHECK(last != it);
  }

  SECTION("end of sequence") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto last = iterator_type();
    std::advance(it, 4);
    CHECK(last == it);
  }

  SECTION("two characters")
  {
    auto bytes = std::string("\xe6\x97\xa5\xd1\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x65e5' == u32(code_point.value()));
    }
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x448' == u32(code_point.value()));
    }
  }

  SECTION("three characters")
  {
    auto bytes = std::string("\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x10346' == u32(code_point.value()));
    }
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x65e5' == u32(code_point.value()));
    }
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x448' == u32(code_point.value()));
    }
  }
}

TEST_CASE("u8 range") {
  using std::begin;
  using std::end;

  SECTION("construction") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = skyr::unicode::view_octet_range(bytes);
    CHECK(begin(view) != end(view));
  }

  SECTION("empty") {
    auto view = skyr::unicode::view_octet_range<std::string>();
    CHECK(begin(view) == end(view));
  }

  SECTION("count") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = skyr::unicode::view_octet_range(bytes);
    CHECK(4 == ranges::size(view));
    CHECK(!ranges::empty(view));
  }

  SECTION("empty count") {
    auto view = skyr::unicode::view_octet_range<std::string>();
    CHECK(0 == ranges::size(view));
    CHECK(ranges::empty(view));
  }

  SECTION("pipe syntax") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = bytes | skyr::unicode::view::u8;
    CHECK(4 == ranges::size(view));
    CHECK(!ranges::empty(view));
  }

  SECTION("pipe syntax with string_view") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto bytes_view = std::string_view(bytes);
    auto view = bytes_view | skyr::unicode::view::u8;
    CHECK(4 == ranges::size(view));
    CHECK(!ranges::empty(view));
  }

  SECTION("pipe syntax invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = bytes | skyr::unicode::view::u8;
    auto it = std::begin(view), last = std::end(view);
    *it++;
    CHECK(it == last);
    CHECK(1 == ranges::size(view));
    CHECK(!ranges::empty(view));
  }

//  SECTION("pipe syntax with u16 string") {
//    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
//    auto view = bytes | skyr::unicode::view::u16;
//    auto u16 = std::u16string(begin(view), end(view));
//    CHECK(u"\xD83C\xDFF3\xFE0F\x200D\xD83C\xDF08" == u16);
//  }

  SECTION("pipe syntax with u32 string") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u32 = skyr::unicode::u32string(bytes | skyr::unicode::view::u32);
    REQUIRE(u32);
    CHECK(U"\x1F3F3\xFE0F\x200D\x1F308" == u32.value());
  }

  SECTION("pipe syntax with u32 string invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u32 = skyr::unicode::u32string(bytes | skyr::unicode::view::u32);
    CHECK(!u32);
  }
}
