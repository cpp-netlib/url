// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_ERROR_HPP
#define SKYR_UNICODE_ERROR_HPP

#include <system_error>

namespace skyr {
inline namespace v1 {
namespace unicode {
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

/// Creates a `std::error_code` given a `skyr::unicode_errc` value
/// \param error A Unicode error
/// \returns A `std::error_code` object
std::error_code make_error_code(unicode_errc error) noexcept;
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::unicode::unicode_errc> : true_type {};
}  // namespace std

#endif //SKYR_UNICODE_ERROR_HPP
