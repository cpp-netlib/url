// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_CODE_POINT_HPP
#define SKYR_UNICODE_CODE_POINT_HPP

#include <tl/expected.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/code_points/u8.hpp>
#include <skyr/unicode/code_points/u16.hpp>


namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline tl::expected<char32_t, std::error_code> u32_value(
    u8_code_point_view<OctetIterator> code_point) noexcept {
  return find_code_point(code_point.begin()).map([] (auto &&state) { return state.value; });
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline tl::expected<char32_t, std::error_code> u32_value(
    tl::expected<u8_code_point_view<OctetIterator>, std::error_code> code_point) noexcept {
  return code_point.and_then([] (auto &&code_point) { return u32_value(code_point); });
}

///
/// \param code_point
/// \return
inline tl::expected<char32_t, std::error_code> u32_value(
    u16_code_point_t code_point) noexcept {
  return code_point.u32_value();
}

///
/// \param code_point
/// \return
inline tl::expected<char32_t, std::error_code> u32_value(
    tl::expected<u16_code_point_t, std::error_code> code_point) noexcept {
  return code_point.and_then([] (auto code_point) { return code_point.u32_value(); });
}

///
/// \param code_point
/// \return
inline tl::expected<char32_t, std::error_code> u32_value(
    char32_t code_point) noexcept {
  return code_point;
}

///
/// \param code_point
/// \return
inline tl::expected<char32_t, std::error_code> u32_value(
    tl::expected<char32_t, std::error_code> code_point) noexcept {
  return code_point;
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline tl::expected<u16_code_point_t, std::error_code> u16_value(
    u8_code_point_view<OctetIterator> code_point) {
  return u16_code_point(u32_value(code_point));
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline tl::expected<u16_code_point_t, std::error_code> u16_value(
    tl::expected<u8_code_point_view<OctetIterator>, std::error_code> code_point) {
  return u32_value(code_point).map([] (auto code_point) { return u16_code_point(code_point); });
}
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif //SKYR_UNICODE_CODE_POINT_HPP
