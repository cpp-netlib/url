// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_all.hpp>
#include <skyr/v2/domain/idna.hpp>


struct parameter {
  char32_t value;
  skyr::idna::idna_status status;
};


TEST_CASE("IDNA character values", "[idna]") {

  auto code_point = GENERATE(
      parameter{0x0000, skyr::idna::idna_status::disallowed_std3_valid},
      parameter{0x002d, skyr::idna::idna_status::valid},
      parameter{0x10fffd, skyr::idna::idna_status::disallowed},
      parameter{0x10ffff, skyr::idna::idna_status::disallowed});

  SECTION("code_point_set") {
    const auto [value, status] = code_point;
    INFO("0x" << std::hex << static_cast<std::uint32_t>(value) << ", " << static_cast<int>(status));
    REQUIRE(status == skyr::idna::code_point_status(value));
  }
}
