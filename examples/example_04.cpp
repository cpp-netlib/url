// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <skyr/core/parse.hpp>
#include <skyr/core/serialize.hpp>

int main() {
  auto base = skyr::parse("https://example.org/");
  auto url = skyr::parse(
      "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", base.value());
  if (url) {
    std::cout << skyr::serialize(url.value()) << std::endl;
  }
}
