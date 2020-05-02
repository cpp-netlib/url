// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_ERRORS_HPP
#define SKYR_V1_UNICODE_ERRORS_HPP

/// \file errors.hpp

#include <system_error>

namespace skyr {
inline namespace v1 {
namespace unicode {
/// \enum unicode_errc
/// Enumerates Unicode errors
enum class unicode_errc {
  /// Overflow
  overflow,
  /// Invalid lead code point
  invalid_lead,
  /// Illegal byte sequence
  illegal_byte_sequence,
  /// Invalid code point
  invalid_code_point,
};

namespace details {
class unicode_error_category : public std::error_category {
 public:
  [[nodiscard]] auto name() const noexcept -> const char * override {
    return "unicode";
  }

  [[nodiscard]] auto message(int error) const noexcept -> std::string override {
    switch (static_cast<unicode_errc>(error)) {
      case unicode_errc::overflow:return "Overflow";
      case unicode_errc::invalid_lead:return "Invalid lead";
      case unicode_errc::illegal_byte_sequence:return "Illegal byte sequence";
      case unicode_errc::invalid_code_point:return "Invalid code point";
      default:return "(Unknown error)";
    }
  }
};
}  // namespace details

/// Creates a `std::error_code` given a `skyr::unicode_errc` value
/// \param error A Unicode error
/// \returns A `std::error_code` object
inline auto make_error_code(unicode_errc error) noexcept -> std::error_code {
  static const details::unicode_error_category category{};
  return std::error_code(static_cast<int>(error), category);
}
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::unicode::unicode_errc> : true_type {};
}  // namespace std

#endif // SKYR_V1_UNICODE_ERRORS_HPP
