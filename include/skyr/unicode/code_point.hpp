// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_CODE_POINT_HPP
#define SKYR_UNICODE_CODE_POINT_HPP

#include <expected>

#include <skyr/unicode/code_points/u16.hpp>
#include <skyr/unicode/code_points/u8.hpp>
#include <skyr/unicode/errors.hpp>

namespace skyr::unicode {
///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
constexpr auto u32_value(u8_code_point_view<OctetIterator> code_point) noexcept
    -> std::expected<char32_t, unicode_errc> {
  constexpr auto to_value = [](auto&& state) { return state.value; };
  return find_code_point(code_point.begin()).transform(to_value);
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
constexpr auto u32_value(std::expected<u8_code_point_view<OctetIterator>, unicode_errc> code_point) noexcept
    -> std::expected<char32_t, unicode_errc> {
  constexpr auto to_u32 = [](auto&& code_point) { return u32_value(code_point); };
  return code_point.and_then(to_u32);
}

///
/// \param code_point
/// \return
constexpr auto u32_value(u16_code_point_t code_point) noexcept -> std::expected<char32_t, unicode_errc> {
  return code_point.u32_value();
}

///
/// \param code_point
/// \return
constexpr auto u32_value(std::expected<u16_code_point_t, unicode_errc> code_point) noexcept
    -> std::expected<char32_t, unicode_errc> {
  constexpr auto to_u32 = [](auto code_point) { return code_point.u32_value(); };
  return code_point.and_then(to_u32);
}

///
/// \param code_point
/// \return
constexpr auto u32_value(char32_t code_point) noexcept -> std::expected<char32_t, unicode_errc> {
  return code_point;
}

///
/// \param code_point
/// \return
constexpr auto u32_value(std::expected<char32_t, unicode_errc> code_point) noexcept
    -> std::expected<char32_t, unicode_errc> {
  return code_point;
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
constexpr auto u16_value(u8_code_point_view<OctetIterator> code_point)
    -> std::expected<u16_code_point_t, unicode_errc> {
  return u16_code_point(u32_value(code_point));
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
constexpr auto u16_value(std::expected<u8_code_point_view<OctetIterator>, unicode_errc> code_point) {
  constexpr auto to_u16 = [](auto code_point) { return u16_code_point(code_point); };
  return u32_value(code_point).transform(to_u16);
}
}  // namespace skyr::unicode

#endif  // SKYR_UNICODE_CODE_POINT_HPP
