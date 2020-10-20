// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_TRAITS_ITERATOR_VALUE_HPP
#define SKYR_UNICODE_TRAITS_ITERATOR_VALUE_HPP

namespace skyr {
inline namespace v1 {
namespace unicode::traits {
///
/// \tparam Iterator
template <class Iterator>
class iterator_value {
 public:
  using type = typename Iterator::value_type;
};

///
/// \tparam T
/// \tparam N
template <typename T>
class iterator_value<T*> {
 public:
  using type = T;
};

template <class Range>
using iterator_value_t = typename iterator_value<Range>::type;
}  // namespace unicode::concepts
}  // namespace v1
}  // namespace skyr

#endif // SKYR_UNICODE_TRAITS_ITERATOR_VALUE_HPP
