// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_CODE_POINTS_U16_HPP
#define SKYR_V1_UNICODE_CODE_POINTS_U16_HPP

#include <skyr/v1/unicode/constants.hpp>
#include <skyr/v1/unicode/core.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <tl/expected.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
class u16_code_point_t {

 public:

  ///
  /// \param code_point
  explicit constexpr u16_code_point_t(char32_t code_point)
      : code_point_(code_point) {}

  ///
  /// \param code_point
  explicit constexpr u16_code_point_t(char16_t code_point)
      : code_point_(code_point) {}

  constexpr u16_code_point_t(char16_t lead_value, char16_t trail_value)
      : code_point_((lead_value << 10U) + trail_value + constants::surrogates::offset) {}

  ///
  /// \return
  [[nodiscard]] constexpr auto is_surrogate_pair() const noexcept {
    return code_point_ > U'\xffff';
  }

  ///
  /// \return
  [[nodiscard]] auto lead_value() const {
    return is_surrogate_pair()?
           static_cast<char16_t>((code_point_ >> 10U) + constants::surrogates::lead_offset) :
           static_cast<char16_t>(code_point_);
  }

  ///
  /// \return
  [[nodiscard]] auto trail_value() const {
    return is_surrogate_pair()?
           static_cast<char16_t>((code_point_ & 0x3ffU) + constants::surrogates::trail_min) :
           0;
  }

  [[nodiscard]] auto u32_value() const noexcept -> tl::expected<char32_t, unicode_errc> {
    return code_point_;
  }

 private:

  char32_t code_point_;

};

///
/// \param code_point
/// \return
inline auto u16_code_point(char32_t code_point) {
  return u16_code_point_t(code_point);
}

///
/// \param code_point
/// \return
inline auto u16_code_point(char16_t code_point) {
  return u16_code_point_t(code_point);
}

///
/// \param lead_code_unit
/// \param trail_code_unit
/// \return
inline auto u16_code_point(
    char16_t lead_code_unit,
    char16_t trail_code_unit) {
  return u16_code_point_t(lead_code_unit, trail_code_unit);
}
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_CODE_POINTS_U16_HPP
