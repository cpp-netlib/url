// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_RANGES_VIEWS_UNCHECKED_U8_VIEW_HPP
#define SKYR_V1_UNICODE_RANGES_VIEWS_UNCHECKED_U8_VIEW_HPP

#include <iterator>
#include <algorithm>
#include <cassert>
#include <tl/expected.hpp>
#include <skyr/v1/unicode/code_point.hpp>
#include <skyr/v1/unicode/core.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <skyr/v1/unicode/traits/range_iterator.hpp>
#include <skyr/v1/unicode/ranges/sentinel.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \tparam OctetIterator
template <class OctetIterator>
class unchecked_u8_range_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = u8_code_point_view<OctetIterator>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_pointer = const value_type *;
  ///
  using pointer = const_reference;
  ///
  using difference_type = std::ptrdiff_t;
  ///
  using size_type = std::size_t;

  ///
  /// \param it
  /// \param last
  constexpr unchecked_u8_range_iterator(OctetIterator it, OctetIterator last)
      : it_(it), last_(last) {}

  ///
  /// \return
  constexpr auto operator ++ (int) noexcept -> unchecked_u8_range_iterator {
    assert(it_ != last_);
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  constexpr auto operator ++ () noexcept -> unchecked_u8_range_iterator & {
    assert(it_ != last_);
    increment();
    return *this;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto operator * () const noexcept -> const_reference {
    assert(it_ != last_);
    auto last = it_;
    std::advance(last, sequence_length(*it_));
    return u8_code_point_view<OctetIterator>(it_, last);
  }

  ///
  /// \param sentinel
  /// \return
  [[nodiscard]] constexpr auto operator == ([[maybe_unused]] sentinel sentinel) const noexcept {
    return it_ == last_;
  }

  ///
  /// \param sentinel
  /// \return
  [[nodiscard]] constexpr auto operator != (sentinel sentinel) const noexcept {
    return !(*this == sentinel);
  }

 private:

  constexpr void increment() {
    std::advance(it_, sequence_length(*it_));
  }

  OctetIterator it_, last_;

};

///
/// \tparam OctetRange
template <class OctetRange>
class view_unchecked_u8_range {

  using octet_iterator_type = traits::range_iterator_t<OctetRange>;
  using iterator_type = unchecked_u8_range_iterator<octet_iterator_type>;

 public:

  ///
  using value_type = u8_code_point_view<octet_iterator_type>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_iterator = iterator_type;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  /// \param range
  explicit constexpr view_unchecked_u8_range(
      const OctetRange &range)
      : it_(std::begin(range), std::end(range)) {}

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return it_;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cend() const noexcept {
    return sentinel{};
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto begin() const noexcept {
    return cbegin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto end() const noexcept {
    return cend();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto empty() const noexcept {
    return begin() == end();
  }

 private:

  iterator_type it_;

};

namespace views {
///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
[[maybe_unused]] constexpr inline auto unchecked_u8(
    const OctetRange &range) {
  return view_unchecked_u8_range{range};
}
}  // namespace views
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_RANGES_VIEWS_UNCHECKED_U8_VIEW_HPP
