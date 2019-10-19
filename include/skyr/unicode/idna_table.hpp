// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_IDNA_TABLE_HPP
#define SKYR_UNICODE_IDNA_TABLE_HPP

namespace skyr {
inline namespace v1 {
namespace unicode {
/// The status values come from the IDNA mapping table in unicode TR46:
///
/// https://unicode.org/reports/tr46/#IDNA_Mapping_Table
///
enum class idna_status {
  /// The code point is disallowed
  disallowed = 1,
  /// The code point is disallowed, but can be treated as valid when using std 3
  /// rules
  disallowed_std3_valid,
  /// The code point is disallowed, but can be mapped to another value when
  /// using std 3 rules
  disallowed_std3_mapped,
  /// The code point will be ignored - equivalent to being mapped to an empty
  /// string
  ignored,
  /// The code point will be replaced by another character
  mapped,
  /// The code point is either mapped or valid, depending on whether the process
  /// is transitional or not
  deviation,
  /// The code point is valid
  valid,
};

///
/// \param code_point A code point value
/// \return The status of the code point
idna_status map_idna_status(char32_t code_point);

///
/// \param code_point A code point value
/// \return The code point or mapped value, depending on the status of the code
/// point
char32_t map_idna_code_point(char32_t code_point);
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_UNICODE_IDNA_TABLE_HPP
