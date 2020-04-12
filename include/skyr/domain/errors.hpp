// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_DOMAIN_ERRORS_HPP
#define SKYR_DOMAIN_ERRORS_HPP

#include <system_error>

namespace skyr {
inline namespace v1 {
/// \enum domain_errc
/// Enumerates domain processing errors
enum class domain_errc {
  /// The domain code point is disallowed
  disallowed_code_point = 1,
  /// The encoder or decoder received bad input
  bad_input,
  /// Overflow
  overflow,
  /// Unicode encoding error
  encoding_error,
};

/// Creates a `std::error_code` given a `skyr::domain_errc` value
/// \param error A domain error
/// \returns A `std::error_code` object
auto make_error_code(domain_errc error) noexcept -> std::error_code;
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::domain_errc> : true_type {};
}  // namespace std

#endif //SKYR_DOMAIN_ERRORS_HPP
