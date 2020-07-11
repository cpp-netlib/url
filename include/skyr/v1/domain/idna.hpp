// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_DOMAIN_IDNA_HPP
#define SKYR_V1_DOMAIN_IDNA_HPP

#include <tl/expected.hpp>
#include <skyr/v1/domain/errors.hpp>
#include <skyr/v1/unicode/traits/range_iterator.hpp>

namespace skyr {
inline namespace v1 {
namespace idna {
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

///
/// \param code_point A code point value
/// \return The status of the code point
auto code_point_status(char32_t code_point) -> idna_status;

///
/// \param code_point A code point value
/// \return The code point or mapped value, depending on the status of the code
/// point
auto map_code_point(char32_t code_point) -> char32_t;

///
/// \tparam FwdIter
/// \param first
/// \param last
/// \param use_std3_ascii_rules
/// \param transitional_processing
/// \return
template <class FwdIter>
inline auto map_code_points(
    FwdIter first,
    FwdIter last,
    bool use_std3_ascii_rules,
    bool transitional_processing) -> tl::expected<FwdIter, domain_errc> {
  for (auto it = first; it != last; ++it) {
    switch (code_point_status(*it)) {
      case idna_status::disallowed:
        return tl::make_unexpected(domain_errc::disallowed_code_point);
      case idna_status::disallowed_std3_valid:
        if (use_std3_ascii_rules) {
          return tl::make_unexpected(domain_errc::disallowed_code_point);
        } else {
          *first++ = *it;
        }
        break;
      case idna_status::disallowed_std3_mapped:
        if (use_std3_ascii_rules) {
          return tl::make_unexpected(domain_errc::disallowed_code_point);
        } else {
          *first++ = map_code_point(*it);
        }
        break;
      case idna_status::ignored:
        break;
      case idna_status::mapped:
        *first++ = idna::map_code_point(*it);
        break;
      case idna_status::deviation:
        if (transitional_processing) {
          *first++ = idna::map_code_point(*it);
        } else {
          *first++ = *it;
        }
        break;
      case idna_status::valid:
        *first++ = *it;
        break;
    }
  }
  return first;
}

template <class T>
inline auto map_code_points(
    T &code_points,
    bool use_std3_ascii_rules,
    bool transitional_processing) -> tl::expected<typename unicode::traits::range_iterator_t<T>, domain_errc> {
  return map_code_points(std::begin(code_points), std::end(code_points), use_std3_ascii_rules, transitional_processing);
}
}  // namespace idna
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_DOMAIN_IDNA_HPP
