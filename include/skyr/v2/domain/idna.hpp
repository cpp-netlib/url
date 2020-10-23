// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_DOMAIN_IDNA_HPP
#define SKYR_V2_DOMAIN_IDNA_HPP

#include <tl/expected.hpp>
#include <skyr/v2/domain/errors.hpp>
#include <skyr/v2/unicode/traits/range_iterator.hpp>
#include <skyr/v2/domain/idna_tables.hpp>

namespace skyr::inline v2::idna {
///
/// \param code_point A code point value
/// \return The status of the code point
constexpr auto code_point_status(char32_t code_point) -> idna_status {
  constexpr auto less = [](const auto &range, auto code_point) { return range.last < code_point; };

  auto first = std::cbegin(details::statuses), last = std::cend(details::statuses);
  auto it = std::lower_bound(first, last, code_point, less);
  return (it == last) || !((code_point >= (*it).first) && (code_point <= (*it).last)) ? idna_status::valid : it->status;
}

namespace details {
constexpr auto map_code_point_16(char16_t code_point) -> char16_t {
  constexpr auto less = [](const auto &lhs, auto rhs) { return lhs.code_point < rhs; };

  auto first = std::cbegin(mapped_16), last = std::cend(mapped_16);
  auto it = std::lower_bound(first, last, code_point, less);
  return (it != last) ? it->mapped : code_point;
}
}  // namespace details

///
/// \param code_point A code point value
/// \return The code point or mapped value, depending on the status of the code
/// point
constexpr auto map_code_point(char32_t code_point) -> char32_t {
  constexpr auto less = [](const auto &lhs, auto rhs) { return lhs.code_point < rhs; };

  if (code_point <= U'\xffff') {
    return static_cast<char32_t>(details::map_code_point_16(static_cast<char16_t>(code_point)));
  }

  auto first = std::cbegin(details::mapped_32), last = std::cend(details::mapped_32);
  auto it = std::lower_bound(first, last, code_point, less);
  return (it != last) ? it->mapped : code_point;
}

///
/// \tparam FwdIter
/// \param first
/// \param last
/// \param use_std3_ascii_rules
/// \param transitional_processing
/// \return
template <class FwdIter>
inline auto map_code_points(FwdIter first, FwdIter last, bool use_std3_ascii_rules, bool transitional_processing)
    -> tl::expected<FwdIter, domain_errc> {
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
inline auto map_code_points(T &code_points, bool use_std3_ascii_rules, bool transitional_processing)
    -> tl::expected<typename unicode::traits::range_iterator_t<T>, domain_errc> {
  return map_code_points(std::begin(code_points), std::end(code_points), use_std3_ascii_rules, transitional_processing);
}
}  // namespace skyr::inline v2::idna

#endif  // SKYR_V2_DOMAIN_IDNA_HPP
