// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <skyr/url.hpp>
#include <skyr/url/percent_encoding/percent_decode_range.hpp>

int main(int argc, char *argv[]) {
  using namespace skyr::percent_encoding;

  auto url = skyr::url("http://example.org/\xf0\x9f\x92\xa9");
  auto value = as<std::string>(url.record().path.back() | view::decode).value();
  std::cout << value << std::endl;
}
