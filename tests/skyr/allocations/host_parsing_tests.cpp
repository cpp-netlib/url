// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <exception>
#include <iostream>
#include <exception>
#include <string_view>
#include <exception>
#include <vector>
#include <exception>
#include <skyr/core/host.hpp>
#include <exception>
#include "allocations.hpp"

using namespace std::string_view_literals;

int main() {
  const auto host_strings = std::vector<std::string_view>{
      "example.com"sv,
      "192.168.0.1"sv,
      "[2001:0db8:0:0::1428:57ab]"sv,
      "localhost"sv,
      "a.b.c.d.e.f.g.h.i.j.k.l.example.com"sv,
      "sub.llanfairpwllgwyngyllgogerychwndrwbwllllantysiliogogogoch.com"sv,
      "i am a terrible host name and n\0t in any way.valid.but. i am useful to validate @llocation"sv};

  for (auto&& host_string : host_strings) {
    SKYR_ALLOCATIONS_START_COUNTING("skyr::parse_host(\"" << host_string << "\")");
    auto host = skyr::parse_host(host_string);
  }
}
