// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_TRAITS_RANGE_ITERATOR_HPP
#define SKYR_UNICODE_TRAITS_RANGE_ITERATOR_HPP

namespace skyr {
inline namespace v1 {
namespace unicode::traits {
///
/// \tparam Range
template <class Range>
class range_iterator {
 public:
  using type = typename Range::const_iterator;
};

///
/// \tparam T
/// \tparam N
template <typename T, std::size_t N>
class range_iterator<T[N]> {
 public:
  using type = const T *;
};
}  // namespace unicode::traits
}  // namespace v1
}  // namespace skyr

#endif // SKYR_UNICODE_TRAITS_RANGE_ITERATOR_HPP
