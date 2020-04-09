// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <skyr/url.hpp>
#include <skyr/percent_encoding/percent_decode.hpp>

int main(int argc, char *argv[]) {
  using skyr::percent_decode;

  auto url = skyr::url("http://example.org/\xf0\x9f\x92\xa9");
  auto value = percent_decode<std::string>(url.record().path.back()).value();
  std::cout << value << std::endl;
}
