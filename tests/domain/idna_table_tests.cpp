// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "../../src/v1/domain/idna.hpp"

TEST_CASE("IDNA character values", "[idna]") {
  using param = std::pair<char32_t, skyr::domain::idna_status>;

  auto code_point = GENERATE(
          param{0x0000, skyr::domain::idna_status::disallowed_std3_valid},
          param{0x002d, skyr::domain::idna_status::valid},
          param{0x10fffd, skyr::domain::idna_status::disallowed},
          param{0x10ffff, skyr::domain::idna_status::disallowed});

  SECTION("code_point_set") {
    const auto [value, mapped] = code_point;
    REQUIRE(mapped == skyr::domain::map_idna_status(value));
  }
}
