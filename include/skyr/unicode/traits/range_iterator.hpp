// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_TRAITS_RANGE_ITERATOR_HPP
#define SKYR_UNICODE_TRAITS_RANGE_ITERATOR_HPP

#include <type_traits>

namespace skyr::unicode::traits {
///
/// \tparam Range
template <class Range>
class range_iterator {
 public:
  using type = typename std::decay_t<Range>::const_iterator;
};

///
/// \tparam T
/// \tparam N
template <typename T, std::size_t N>
class range_iterator<T[N]> {  // NOLINT
 public:
  using type = const T *;
};

template <class Range>
using range_iterator_t = typename range_iterator<Range>::type;
}  // namespace skyr::unicode::traits

#endif  // SKYR_UNICODE_TRAITS_RANGE_ITERATOR_HPP
