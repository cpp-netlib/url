// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_ERRORS_HPP
#define SKYR_V1_UNICODE_ERRORS_HPP

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
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_ERRORS_HPP
