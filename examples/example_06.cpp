// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <skyr/url.hpp>
#include <skyr/url/percent_encoding/percent_decode_range.hpp>

int main(int argc, char *argv[]) {
  auto decode_pathname = [] (const auto &url) {
    return skyr::percent_encoding::as<std::string>(
        url.record().path.back() | skyr::percent_encoding::view::decode);
  };

  auto value = skyr::make_url("http://example.org/\xf0\x9f\x92\xa9")
      .and_then(decode_pathname)
      .value()
      ;
  std::cout << value << std::endl;
}
