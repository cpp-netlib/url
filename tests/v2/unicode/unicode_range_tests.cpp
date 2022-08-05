// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <string_view>
#include <catch2/catch_all.hpp>
#include <skyr/v2/unicode/ranges/transforms/u16_transform.hpp>
#include <skyr/v2/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/v2/unicode/ranges/transforms/u8_transform.hpp>
#include <skyr/v2/unicode/ranges/views/u16_view.hpp>
#include <skyr/v2/unicode/ranges/views/u8_view.hpp>

namespace unicode = skyr::unicode;


TEST_CASE("octet range iterator") {
  using iterator_type = unicode::u8_range_iterator<std::string_view::const_iterator>;
  using namespace std::string_view_literals;

  SECTION("construction") {
    const auto bytes = "\xf0\x9f\x92\xa9"sv;
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F4A9' == u32_value(code_point.value()));
  }

  SECTION("construction from array") {
    const char bytes[] = "\xf0\x9f\x92\xa9";
    auto first = std::begin(bytes), last = std::end(bytes);
    auto it = unicode::u8_range_iterator<const char *>(first, last);
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F4A9' == u32_value(code_point.value()));
  }

  SECTION("increment") {
    const auto bytes = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
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
    const auto bytes = "\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    CHECK(!*it);
  }

//  SECTION("equality") {
//    const auto bytes = "\xf0\x9f\x92\xa9"sv;
//    auto it = iterator_type(std::begin(bytes), std::end(bytes));
//    auto last = iterator_type();
//    ++it;
//    CHECK(last == it);
//  }

//  SECTION("inequality") {
//    const auto bytes = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
//    auto it = iterator_type(std::begin(bytes), std::end(bytes));
//    auto last = iterator_type();
//    CHECK(last != it);
//  }

  SECTION("end of sequence") {
    const auto bytes = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
//    auto last = iterator_type();
    std::advance(it, 4);
    CHECK(it == skyr::unicode::sentinel{});
  }

  SECTION("two characters") {
    const auto bytes = "\xe6\x97\xa5\xd1\x88"sv;
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
    const auto bytes = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88"sv;
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
  using namespace std::string_view_literals;

  SECTION("construction") {
    const auto bytes = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto view = unicode::view_u8_range(bytes);
    CHECK(begin(view) != end(view));
  }

//  SECTION("empty") {
//    auto view = unicode::view_u8_range<std::string>();
//    CHECK(begin(view) == end(view));
//  }

//  SECTION("count") {
//    const auto bytes = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
//    auto view = unicode::view_u8_range(bytes);
//    CHECK(4 == view.size());
//    CHECK(!view.empty());
//  }

//  SECTION("empty count") {
//    auto view = unicode::view_u8_range<std::string>();
//    CHECK(view.empty());
//  }

  SECTION("pipe syntax") {
    const auto bytes = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto view = unicode::views::as_u8(bytes);
//    CHECK(4 == view.size());
    CHECK(!view.empty());
  }

  SECTION("pipe syntax with string_view") {
    const auto bytes = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto view = unicode::views::as_u8(std::string_view(bytes));
//    CHECK(4 == view.size());
    CHECK(!view.empty());
  }

  SECTION("pipe syntax invalid") {
    const auto bytes = "\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto view = unicode::views::as_u8(bytes);
    auto it = std::begin(view);
    auto last = std::end(view);
    CHECK(!*it++);
    CHECK(it != last);
//    CHECK(1 == view.size());
    CHECK(!view.empty());
  }

  SECTION("pipe syntax with u16 string") {
    const auto bytes = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto u16 = unicode::as<std::u16string>(unicode::views::as_u8(bytes) | unicode::transforms::to_u16);
    REQUIRE(u16);
    CHECK(u"\xD83C\xDFF3\xFE0F\x200D\xD83C\xDF08" == u16.value());
  }

  SECTION("pipe syntax with u32 string") {
    const auto bytes = "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto u32 = unicode::as<std::u32string>(unicode::views::as_u8(bytes) | unicode::transforms::to_u32);
    REQUIRE(u32);
    CHECK(U"\x1F3F3\xFE0F\x200D\x1F308" == u32.value());
  }

  SECTION("pipe syntax with u16 string invalid") {
    const auto bytes = "\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto u16 = unicode::as<std::u16string>(unicode::views::as_u8(bytes) | unicode::transforms::to_u16);
    CHECK(!u16);
  }

  SECTION("pipe syntax with u32 string invalid") {
    const auto bytes = "\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88"sv;
    auto u32 = unicode::as<std::u32string>(unicode::views::as_u8(bytes) | unicode::transforms::to_u32);
    CHECK(!u32);
  }
}

TEST_CASE("write bytes") {
  using namespace std::string_literals;

  SECTION("bytes from u32") {
    const auto input = U"\x1F3F3\xFE0F\x200D\x1F308"s;
    auto bytes = unicode::as<std::string>(
        input | unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }

  SECTION("vector of bytes from u32") {
    const auto input = U"\x1F3F3\xFE0F\x200D\x1F308"s;
    auto bytes = unicode::as<std::vector<std::byte>>(
        input | unicode::transforms::to_u8);
    REQUIRE(bytes);
  }

  SECTION("bytes from u16") {
    auto input = u"\xD83C\xDFF3\xFE0F\x200D\xD83C\xDF08"s;
    auto bytes = unicode::as<std::string>(
        unicode::views::as_u16(input) | unicode::transforms::to_u8);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }
}
