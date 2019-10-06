// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <skyr/url.hpp>

int main(int argc, char *argv[]) {
  auto url = skyr::make_url("http://example.org/\xf0\x9f\x92\xa9");
  std::cout << url.value().href() << std::endl;
  std::cout << url.value().pathname() << std::endl;
}
