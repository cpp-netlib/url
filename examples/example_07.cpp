// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/url.hpp>
#include <iostream>
#include <skyr/url/percent_encoding/percent_decode_range.hpp>

int main(int argc, char *argv[]) {
  using namespace skyr::percent_encoding;

  auto url =
      skyr::make_url("https://example.org/\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88")
      .and_then([] (auto &&url) { return as<std::string>(url | view::decode); })
      .value();
  std::cout << url << std::endl;
}
