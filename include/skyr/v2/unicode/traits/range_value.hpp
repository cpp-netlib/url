// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_UNICODE_TRAITS_RANGE_VALUE_HPP
#define SKYR_V2_UNICODE_TRAITS_RANGE_VALUE_HPP

#include <cstdlib>

namespace skyr::inline v2::unicode::traits {
///
/// \tparam Range
template <class Range>
class range_value {
 public:
  using type = typename Range::value_type;
};

///
/// \tparam T
/// \tparam N
template <typename T, std::size_t N>
class range_value<T[N]> {  // NOLINT
 public:
  using type = T;
};

template <class Range>
using range_value_t = typename range_value<Range>::type;
}  // namespace skyr::inline v2::unicode::traits

#endif  // SKYR_V2_UNICODE_TRAITS_RANGE_VALUE_HPP
