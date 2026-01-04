// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_CODE_POINTS_U16_HPP
#define SKYR_UNICODE_CODE_POINTS_U16_HPP

#include <expected>

#include <skyr/unicode/constants.hpp>
#include <skyr/unicode/core.hpp>
#include <skyr/unicode/errors.hpp>

namespace skyr::unicode {
///
class u16_code_point_t {
 public:
  ///
  /// \param code_point
  explicit constexpr u16_code_point_t(char32_t code_point) : code_point_(code_point) {
  }

  ///
  /// \param code_point
  explicit constexpr u16_code_point_t(char16_t code_point) : code_point_(code_point) {
  }

  constexpr u16_code_point_t(char16_t lead_value, char16_t trail_value)
      : code_point_((lead_value << 10U) + trail_value +
                    constants::surrogates::offset) {  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto is_surrogate_pair() const noexcept {
    return code_point_ > U'\xffff';
  }

  ///
  /// \return
  [[nodiscard]] auto lead_value() const {
    return is_surrogate_pair()
               ? static_cast<char16_t>(
                     (code_point_ >> 10U) +
                     constants::surrogates::lead_offset)  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
               : static_cast<char16_t>(code_point_);
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto trail_value() const {
    return is_surrogate_pair() ? static_cast<char16_t>((code_point_ & 0x3ffU) + constants::surrogates::trail_min)
                               : 0;  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
  }

  [[nodiscard]] constexpr auto u32_value() const noexcept -> std::expected<char32_t, unicode_errc> {
    return code_point_;
  }

 private:
  char32_t code_point_;
};

///
/// \param code_point
/// \return
constexpr auto u16_code_point(char32_t code_point) {
  return u16_code_point_t(code_point);
}

///
/// \param code_point
/// \return
constexpr auto u16_code_point(char16_t code_point) {
  return u16_code_point_t(code_point);
}

///
/// \param lead_code_unit
/// \param trail_code_unit
/// \return
constexpr auto u16_code_point(char16_t lead_code_unit, char16_t trail_code_unit) {
  return u16_code_point_t(lead_code_unit, trail_code_unit);
}
}  // namespace skyr::unicode

#endif  // SKYR_UNICODE_CODE_POINTS_U16_HPP
