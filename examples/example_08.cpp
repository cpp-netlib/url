// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <skyr/url.hpp>
#include <skyr/percent_encoding/percent_decode.hpp>

int main(int argc, char *argv[]) {
  using skyr::percent_decode;

  auto url = skyr::url(
      "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
  url.search_parameters().sort();
  for (auto [name, value] : url.search_parameters()) {
    auto decoded_value = percent_decode<std::string>(value).value();
    std::cout << name << ": " << decoded_value << std::endl;
  }
}
