// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_CODE_POINT_HPP
#define SKYR_V1_UNICODE_CODE_POINT_HPP

#include <skyr/v1/unicode/code_points/u16.hpp>
#include <skyr/v1/unicode/code_points/u8.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <tl/expected.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline auto u32_value(
    u8_code_point_view<OctetIterator> code_point) noexcept -> tl::expected<char32_t, std::error_code> {
  return find_code_point(code_point.begin()).map([] (auto &&state) { return state.value; });
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline auto u32_value(
    tl::expected<u8_code_point_view<OctetIterator>, std::error_code> code_point) noexcept
    -> tl::expected<char32_t, std::error_code> {
  constexpr static auto to_u32 = [] (auto &&code_point) { return u32_value(code_point); };
  return code_point.and_then(to_u32);
}

///
/// \param code_point
/// \return
inline auto u32_value(
    u16_code_point_t code_point) noexcept -> tl::expected<char32_t, std::error_code> {
  return code_point.u32_value();
}

///
/// \param code_point
/// \return
inline auto u32_value(
    tl::expected<u16_code_point_t, std::error_code> code_point) noexcept
    -> tl::expected<char32_t, std::error_code> {
  constexpr static auto to_u32 = [] (auto code_point) { return code_point.u32_value(); };
  return code_point.and_then(to_u32);
}

///
/// \param code_point
/// \return
inline auto u32_value(
    char32_t code_point) noexcept -> tl::expected<char32_t, std::error_code> {
  return code_point;
}

///
/// \param code_point
/// \return
inline auto u32_value(
    tl::expected<char32_t, std::error_code> code_point) noexcept -> tl::expected<char32_t, std::error_code> {
  return code_point;
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline auto u16_value(
    u8_code_point_view<OctetIterator> code_point) -> tl::expected<u16_code_point_t, std::error_code> {
  return u16_code_point(u32_value(code_point));
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline auto u16_value(
    tl::expected<u8_code_point_view<OctetIterator>, std::error_code> code_point) {
  constexpr static auto to_u16 = [] (auto code_point) { return u16_code_point(code_point); };
  return u32_value(code_point).map(to_u16);
}
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_CODE_POINT_HPP
