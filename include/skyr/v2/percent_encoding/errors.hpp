// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_PERCENT_ENCODING_ERRORS_HPP
#define SKYR_V1_PERCENT_ENCODING_ERRORS_HPP

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
}  // namespace percent_encoding
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_PERCENT_ENCODING_ERRORS_HPP
