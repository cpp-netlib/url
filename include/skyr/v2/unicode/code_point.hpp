// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_UNICODE_CODE_POINT_HPP
#define SKYR_V2_UNICODE_CODE_POINT_HPP

#include <tl/expected.hpp>
#include <skyr/v2/unicode/code_points/u16.hpp>
#include <skyr/v2/unicode/code_points/u8.hpp>
#include <skyr/v2/unicode/errors.hpp>

namespace skyr::inline v2::unicode {
///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
constexpr inline auto u32_value(u8_code_point_view<OctetIterator> code_point) noexcept
    -> tl::expected<char32_t, unicode_errc> {
  constexpr auto to_value = [](auto &&state) { return state.value; };
  return find_code_point(code_point.begin()).map(to_value);
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
constexpr inline auto u32_value(tl::expected<u8_code_point_view<OctetIterator>, unicode_errc> code_point) noexcept
    -> tl::expected<char32_t, unicode_errc> {
  constexpr auto to_u32 = [](auto &&code_point) { return u32_value(code_point); };
  return code_point.and_then(to_u32);
}

///
/// \param code_point
/// \return
constexpr inline auto u32_value(u16_code_point_t code_point) noexcept -> tl::expected<char32_t, unicode_errc> {
  return code_point.u32_value();
}

///
/// \param code_point
/// \return
constexpr inline auto u32_value(tl::expected<u16_code_point_t, unicode_errc> code_point) noexcept
    -> tl::expected<char32_t, unicode_errc> {
  constexpr auto to_u32 = [](auto code_point) { return code_point.u32_value(); };
  return code_point.and_then(to_u32);
}

///
/// \param code_point
/// \return
constexpr inline auto u32_value(char32_t code_point) noexcept -> tl::expected<char32_t, unicode_errc> {
  return code_point;
}

///
/// \param code_point
/// \return
constexpr inline auto u32_value(tl::expected<char32_t, unicode_errc> code_point) noexcept
    -> tl::expected<char32_t, unicode_errc> {
  return code_point;
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
constexpr inline auto u16_value(u8_code_point_view<OctetIterator> code_point)
    -> tl::expected<u16_code_point_t, unicode_errc> {
  return u16_code_point(u32_value(code_point));
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
constexpr inline auto u16_value(tl::expected<u8_code_point_view<OctetIterator>, unicode_errc> code_point) {
  constexpr auto to_u16 = [](auto code_point) { return u16_code_point(code_point); };
  return u32_value(code_point).map(to_u16);
}
}  // namespace skyr::inline v2::unicode

#endif  // SKYR_V2_UNICODE_CODE_POINT_HPP
