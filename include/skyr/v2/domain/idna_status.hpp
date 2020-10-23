// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_DOMAIN_IDNA_STATUS_HPP
#define SKYR_V2_DOMAIN_IDNA_STATUS_HPP

namespace skyr::inline v2::idna {
/// \enum idna_status
/// The status values come from the IDNA mapping table in domain TR46:
///
/// https://domain.org/reports/tr46/#IDNA_Mapping_Table
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
}  // namespace skyr::inline v2::idna

#endif  // SKYR_V2_DOMAIN_IDNA_STATUS_HPP
