// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_DOMAIN_ERRORS_HPP
#define SKYR_V2_DOMAIN_ERRORS_HPP

namespace skyr::inline v2 {
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
  /// Invalid domain name length
  invalid_length,
  /// Empty domain
  empty_string,
  /// The number of labels in the domain is too large
  too_many_labels,
};
}  // namespace skyr::inline v2

#endif  // SKYR_V2_DOMAIN_ERRORS_HPP
