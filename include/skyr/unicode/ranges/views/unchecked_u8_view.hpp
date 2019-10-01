// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_UNCHECKED_U8_RANGE_HPP
#define SKYR_UNICODE_UNCHECKED_U8_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>
#include <skyr/unicode/code_point.hpp>
#include <skyr/unicode/ranges/traits.hpp>

namespace skyr::unicode {
///
/// \tparam OctetIterator
template <typename OctetIterator>
class unchecked_u8_range_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = u8_code_point_t<OctetIterator>;
  ///
  using reference = value_type;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  constexpr unchecked_u8_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr unchecked_u8_range_iterator(OctetIterator it)
      : it_(it) {}
  ///
  constexpr unchecked_u8_range_iterator(const unchecked_u8_range_iterator&) = default;
  ///
  constexpr unchecked_u8_range_iterator(unchecked_u8_range_iterator&&) noexcept = default;
  ///
  constexpr unchecked_u8_range_iterator &operator=(const unchecked_u8_range_iterator&) = default;
  ///
  constexpr unchecked_u8_range_iterator &operator=(unchecked_u8_range_iterator&&) noexcept = default;
  ///
  ~unchecked_u8_range_iterator() = default;

  ///
  /// \return
  unchecked_u8_range_iterator operator ++ (int) {
    auto result = *this;
    std::advance(it_, sequence_length(*it_));
    return result;
  }

  ///
  /// \return
  unchecked_u8_range_iterator &operator ++ () {
    std::advance(it_, sequence_length(*it_));
    return *this;
  }

  ///
  /// \return
  constexpr reference operator * () const noexcept {
    return u8_code_point_t<OctetIterator>(
        it_,
        it_ + sequence_length(*it_));
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator == (const unchecked_u8_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator != (const unchecked_u8_range_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  OctetIterator it_;

};

///
/// \tparam OctetRange
template <class OctetRange>
class view_unchecked_u8_range {

  using octet_iterator_type = typename traits::iterator<OctetRange>::type ;
  using iterator_type = unchecked_u8_range_iterator<octet_iterator_type>;

 public:

  ///
  using value_type = u8_code_point_t<octet_iterator_type>;
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

  /// Default constructor
  constexpr view_unchecked_u8_range() = default;

  ///
  /// \param range
  explicit constexpr view_unchecked_u8_range(const OctetRange &range)
      : impl_(
      impl(std::begin(range),
           std::end(range))) {}

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return impl_? impl_.value().first : iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return impl_? impl_.value().last : iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return begin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cend() const noexcept {
    return end();
  }

  ///
  /// \return
  [[nodiscard]] constexpr bool empty() const noexcept {
    return begin() == end();
  }

  ///
  /// \return
  [[nodiscard]] size_type size() const noexcept {
    return static_cast<size_type>(std::distance(begin(), end()));
  }

 private:

  struct impl {
    constexpr impl(
        octet_iterator_type first,
        octet_iterator_type last)
        : first(first)
        , last(last) {}
    iterator_type first, last;
  };

  std::optional<impl> impl_;

};

///
struct unchecked_u8_range_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(OctetRange &&range) const {
    return view_unchecked_u8_range{std::forward<OctetRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(OctetRange &&range, const unchecked_u8_range_fn&) {
    return view_unchecked_u8_range{std::forward<OctetRange>(range)};
  }
};

namespace view {
///
static constexpr unchecked_u8_range_fn unchecked_u8;
}  // namespace view
}  // namespace skyr::unicode

#endif //SKYR_UNICODE_UNCHECKED_U8_RANGE_HPP
