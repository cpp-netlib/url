// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <skyr/v1/domain/idna.hpp>


TEST_CASE("IDNA character values", "[idna]") {
  using param = std::pair<char32_t, skyr::idna::idna_status>;

  auto code_point = GENERATE(
          param{0x0000, skyr::idna::idna_status::disallowed_std3_valid},
          param{0x002d, skyr::idna::idna_status::valid},
          param{0x10fffd, skyr::idna::idna_status::disallowed},
          param{0x10ffff, skyr::idna::idna_status::disallowed});

  SECTION("code_point_set") {
    const auto [value, status] = code_point;
    INFO("0x" << std::hex << value << ", " << static_cast<int>(status));
    REQUIRE(status == skyr::idna::code_point_status(value));
  }
}
