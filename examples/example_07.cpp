// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/url.hpp>
#include <iostream>
#include <skyr/percent_encoding/percent_decode.hpp>

int main() {
  auto url = skyr::url(
      "https://example.org/\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
  std::cout << skyr::percent_decode(url.href()).value() << std::endl;
}
