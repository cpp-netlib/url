// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_U8_RANGE_HPP
#define SKYR_UNICODE_U8_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <cassert>
#include <tl/expected.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>
#include <skyr/unicode/code_point.hpp>
#include <skyr/unicode/traits/iterator_value.hpp>
#include <skyr/unicode/traits/range_iterator.hpp>
#include <skyr/unicode/traits/range_value.hpp>
#include <skyr/unicode/ranges/views/unchecked_u8_view.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
/// \brief
///
/// \tparam OctetIterator
template <typename OctetIterator>
class u8_range_iterator {

  using iterator_type = unchecked_u8_range_iterator<OctetIterator>;

 public:

  /// \c std::forward_iterator_tag
  using iterator_category = std::forward_iterator_tag;
  /// An expected value containing a UTF-8 encoded code point
  using value_type = tl::expected<typename traits::iterator_value<iterator_type>::type, std::error_code>;
  /// A reference type
  using reference = value_type;
  /// A pointer type
  using pointer = typename std::add_pointer<value_type>::type;
  /// \c std::ptrdiff_t
  using difference_type = std::ptrdiff_t;

  /// \brief Constructs an end range iterator
  constexpr u8_range_iterator() = default;
  /// \brief Constructs a \c u8_range_iterator from a range of octets
  ///
  /// \param first
  /// \param last
  explicit constexpr u8_range_iterator(
      OctetIterator first,
      OctetIterator last)
      : it_(iterator_type(first, last))
      , last_(iterator_type()) {}

  /// \brief Post-increment operator
  ///
  /// Increments through a code point
  ///
  /// \return The previous iterator value
  u8_range_iterator operator ++ (int) noexcept {
    assert(it_);
    auto result = *this;
    increment();
    return result;
  }

  /// \brief Pre-increment operator
  ///
  /// Increments through a code point
  ///
  /// \return \c *this
  u8_range_iterator &operator ++ () noexcept {
    assert(it_);
    increment();
    return *this;
  }

  /// \brief Dereference operator
  ///
  /// Returns a proxy to a UTF-8 encoded code point
  ///
  /// \return A proxy to a UTF-8 encoded code point
  constexpr reference operator * () const noexcept {
    assert(it_);
    return checked_u8_code_point(*it_.value());
  }

  /// \brief Equality operator
  ///
  /// \param other Another iterator
  /// \return \c true if the iterators are the same, \c false otherwise
  constexpr bool operator == (const u8_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  /// \brief Inequality operator
  ///
  /// \param other Another iterator
  /// \return `!(*this == other)`
  constexpr bool operator != (const u8_range_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    if (**this) {
      ++it_.value();
      if (it_ == last_) {
        it_ = std::nullopt;
      }
    } else {
      it_ = std::nullopt;
    }
  }

  std::optional<iterator_type> it_, last_;

};

///
/// \tparam OctetRange
template <class OctetRange>
class view_u8_range {

  using octet_iterator_type = typename traits::range_iterator<OctetRange>::type;
  using iterator_type = u8_range_iterator<octet_iterator_type>;

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
  /// \c std::size_t
  using size_type = std::size_t;

  ///
  constexpr view_u8_range() noexcept = default;

  ///
  /// \param range
  explicit constexpr view_u8_range(const OctetRange &range) noexcept
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
    return iterator_type();
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
  [[nodiscard]] constexpr size_type size() const noexcept {
    return static_cast<size_type>(std::distance(begin(), end()));
  }

 private:

  struct impl {
    constexpr impl(
        octet_iterator_type first,
        octet_iterator_type last)
        : first(first, last)
        , last(last, last) {}
    iterator_type first, last;
  };

  std::optional<impl> impl_;

};

namespace view {
///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
inline auto as_u8(
    const OctetRange &range) noexcept {
  static_assert(sizeof(typename traits::range_value<OctetRange>::type) == 1);
  return view_u8_range{range};
}
}  // namespace view
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_UNICODE_U8_RANGE_HPP
