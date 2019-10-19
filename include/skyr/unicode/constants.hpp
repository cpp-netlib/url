// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_CONSTANTS_HPP
#define SKYR_UNICODE_CONSTANTS_HPP

namespace skyr {
inline namespace v1 {
namespace unicode::constants {
namespace surrogates {
// Leading (high) surrogates: 0xd800 - 0xdbff
constexpr char16_t lead_min = 0xd800u;
constexpr char16_t lead_max = 0xdbffu;
// Trailing (low) surrogates: 0xdc00 - 0xdfff
constexpr char16_t trail_min = 0xdc00u;
constexpr char16_t trail_max = 0xdfffu;
constexpr char16_t lead_offset = lead_min - (0x10000u >> 10u);
constexpr char32_t offset = 0x10000u - (lead_min << 10u) - trail_min;
}  // namespace surrogates

namespace code_points {
// Maximum valid value for a Unicode code point
constexpr char32_t max = 0x0010ffffu;
}  // namespace code_points

constexpr char bom[] = {'\xef', '\xbb', '\xbf'};
}  // namespace unicode::constants
}  // namespace v1
}  // namespace skyr

#endif //SKYR_UNICODE_CONSTANTS_HPP
