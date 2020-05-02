// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_CONSTANTS_HPP
#define SKYR_V1_UNICODE_CONSTANTS_HPP

#include <array>

namespace skyr {
inline namespace v1 {
namespace unicode::constants {
namespace surrogates {
// Leading (high) surrogates: 0xd800 - 0xdbff
constexpr char16_t lead_min = u'\xd800';
constexpr char16_t lead_max = u'\xdbff';
// Trailing (low) surrogates: 0xdc00 - 0xdfff
constexpr char16_t trail_min = u'\xdc00';
constexpr char16_t trail_max = u'\xdfff';
constexpr char16_t lead_offset = lead_min - (U'\x10000' >> 10u);
constexpr char32_t offset = U'\x10000' - (lead_min << 10u) - trail_min;
}  // namespace surrogates

namespace code_points {
// Maximum valid value for a Unicode code point
constexpr char32_t max = U'\x0010ffff';
}  // namespace code_points

constexpr std::array<uint8_t, 3> bom = {0xef, 0xbb, 0xbf};
}  // namespace unicode::constants
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_CONSTANTS_HPP
