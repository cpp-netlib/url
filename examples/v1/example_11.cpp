// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string_view>
#include <skyr/core/host.hpp>

using namespace std::string_view_literals;

int main() {
  auto host_domain = skyr::parse_host("example.com"sv);
  std::cout << host_domain.value().serialize() << std::endl;

  auto host_ipv4 = skyr::parse_host("192.168.0.1"sv);
  std::cout << host_ipv4.value().serialize() << std::endl;

  auto host_ipv6 = skyr::parse_host("[2001:0db8:0:0::1428:57ab]"sv);
  std::cout << host_ipv6.value().serialize() << std::endl;

  auto localhost = skyr::parse_host("localhost"sv);
  std::cout << localhost.value().serialize() << std::endl;
}
