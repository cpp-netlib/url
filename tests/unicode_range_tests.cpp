// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <string_view>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <range/v3/empty.hpp>
#include <skyr/unicode/range/views/u8_view.hpp>
#include <skyr/unicode/range/views/u16_view.hpp>
#include <skyr/unicode/range/transforms/u16_transform.hpp>
#include <skyr/unicode/range/transforms/u32_transform.hpp>
#include <skyr/unicode/range/transforms/byte_transform.hpp>


TEST_CASE("octet range iterator") {
  using iterator_type = skyr::unicode::u8_range_iterator<std::string::const_iterator>;

  SECTION("construction") {
    auto bytes = std::string("\xf0\x9f\x92\xa9");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F4A9' == u32_value(code_point.value()));
  }

  SECTION("construction from array") {
    using iterator_type = skyr::unicode::u8_range_iterator<char *>;

    char bytes[] = "\xf0\x9f\x92\xa9";
    auto first = std::begin(bytes), last = std::end(bytes);
    auto it = iterator_type(first, last);
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F4A9' == u32_value(code_point.value()));
  }

  SECTION("increment") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F3F3' == u32_value(code_point.value()));
    ++it;
    code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\xFE0F' == u32_value(code_point.value()));
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
      CHECK(U'\x65e5' == u32_value(code_point.value()));
    }
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x448' == u32_value(code_point.value()));
    }
  }

  SECTION("three characters")
  {
    auto bytes = std::string("\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x10346' == u32_value(code_point.value()));
    }
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x65e5' == u32_value(code_point.value()));
    }
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x448' == u32_value(code_point.value()));
    }
  }
}

TEST_CASE("u8 range") {
  using std::begin;
  using std::end;

  SECTION("construction") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = skyr::unicode::view_u8_range(bytes);
    CHECK(begin(view) != end(view));
  }

  SECTION("empty") {
    auto view = skyr::unicode::view_u8_range<std::string>();
    CHECK(begin(view) == end(view));
  }

  SECTION("count") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = skyr::unicode::view_u8_range(bytes);
    CHECK(4 == ranges::size(view));
    CHECK(!ranges::empty(view));
  }

  SECTION("empty count") {
    auto view = skyr::unicode::view_u8_range<std::string>();
    CHECK(0 == ranges::size(view));
    CHECK(ranges::empty(view));
  }

  SECTION("pipe syntax") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = bytes | skyr::unicode::view::as_u8;
    CHECK(4 == ranges::size(view));
    CHECK(!ranges::empty(view));
  }

  SECTION("pipe syntax with string_view") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = std::string_view(bytes) | skyr::unicode::view::as_u8;
    CHECK(4 == ranges::size(view));
    CHECK(!ranges::empty(view));
  }

  SECTION("pipe syntax invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = bytes | skyr::unicode::view::as_u8;
    auto it = std::begin(view), last = std::end(view);
    CHECK(!*it++);
    CHECK(it == last);
    CHECK(1 == ranges::size(view));
    CHECK(!ranges::empty(view));
  }

  SECTION("pipe syntax with u16 string") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u16 = skyr::unicode::as<std::u16string>(bytes | skyr::unicode::view::as_u8 | skyr::unicode::transform::to_u16);
    REQUIRE(u16);
    CHECK(u"\xD83C\xDFF3\xFE0F\x200D\xD83C\xDF08" == u16.value());
  }

  SECTION("pipe syntax with u32 string") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u32 = skyr::unicode::as<std::u32string>(bytes | skyr::unicode::view::as_u8 | skyr::unicode::transform::to_u32);
    REQUIRE(u32);
    CHECK(U"\x1F3F3\xFE0F\x200D\x1F308" == u32.value());
  }

  SECTION("pipe syntax with u16 string invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u16 = skyr::unicode::as<std::u16string>(bytes | skyr::unicode::view::as_u8 | skyr::unicode::transform::to_u16);
    CHECK(!u16);
  }

  SECTION("pipe syntax with u32 string invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u32 = skyr::unicode::as<std::u32string>(bytes | skyr::unicode::view::as_u8 | skyr::unicode::transform::to_u32);
    CHECK(!u32);
  }
}

TEST_CASE("write bytes") {
  SECTION("bytes from u32") {
    auto input = std::u32string(U"\x1F3F3\xFE0F\x200D\x1F308");
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::transform::to_bytes);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }

  SECTION("bytes from u16") {
    auto input = std::u16string(u"\xD83C\xDFF3\xFE0F\x200D\xD83C\xDF08");
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::view::as_u16 | skyr::unicode::transform::to_bytes);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }

  SECTION("bytes from u16 (2)") {
    auto input = std::u16string(u"\xD83C\xDFF3\xFE0F\x200D\xD83C\xDF08");
    auto bytes = skyr::unicode::as<std::string>(
        input | skyr::unicode::view::as_u16 | skyr::unicode::transform::to_u32 | skyr::unicode::transform::to_bytes);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }
}
