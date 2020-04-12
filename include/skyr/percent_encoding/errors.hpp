// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_PERCENT_ENCODING_ERRORS_HPP
#define SKYR_PERCENT_ENCODING_ERRORS_HPP

#include <system_error>
#include <type_traits>

namespace skyr {
inline namespace v1 {
namespace percent_encoding {
  /// Enumerates percent encoding errors
  enum class percent_encode_errc {
    /// Input was not a hex value
    non_hex_input,
    /// Overflow
    overflow,
    };

  /// Creates a `std::error_code` given a `skyr::percent_encode_errc`
  /// value
  /// \param error A percent encoding error
  /// \returns A `std::error_code` object
  auto make_error_code(percent_encode_errc error) noexcept -> std::error_code;
}  // namespace percent_encoding
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::percent_encoding::percent_encode_errc>
    : true_type {};
}  // namespace std

#endif  // SKYR_PERCENT_ENCODING_ERRORS_HPP
