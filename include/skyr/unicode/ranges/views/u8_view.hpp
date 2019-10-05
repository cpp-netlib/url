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
#include <skyr/unicode/ranges/traits.hpp>
#include <skyr/unicode/ranges/views/unchecked_u8_view.hpp>

namespace skyr::unicode {
///
/// \tparam OctetIterator
template <typename OctetIterator>
class u8_range_iterator {

  using iterator_type = unchecked_u8_range_iterator<OctetIterator>;

 public:

  ///
  using iterator_category = typename iterator_type::iterator_category;
  ///
  using value_type = tl::expected<typename iterator_type::value_type, std::error_code>;
  ///
  using reference = value_type;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  ///
  using difference_type = typename iterator_type::difference_type;

  ///
  constexpr u8_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr u8_range_iterator(
      OctetIterator it,
      OctetIterator last)
      : it_(it)
      , last_(last) {}

  ///
  /// \return
  u8_range_iterator operator ++ (int) noexcept {
    assert(it_);
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  u8_range_iterator &operator ++ () noexcept {
    assert(it_);
    increment();
    return *this;
  }

  ///
  /// \return
  constexpr reference operator * () const noexcept {
    assert(it_);
    return valid_u8_code_point(*it_.value());
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator == (const u8_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
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

  using octet_iterator_type = typename traits::iterator<OctetRange>::type;
  using iterator_type = u8_range_iterator<octet_iterator_type>;

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
  [[nodiscard]] size_type size() const noexcept {
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

///
struct u8_range_fn {

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(
      OctetRange &&range) const noexcept {
    return view_u8_range{std::forward<OctetRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(
      OctetRange &&range,
      const u8_range_fn&) noexcept {
    return view_u8_range{std::forward<OctetRange>(range)};
  }
};

namespace view {
///
static constexpr u8_range_fn as_u8;
}  // namespace view
}  // namespace skyr::unicode

#endif // SKYR_UNICODE_U8_RANGE_HPP
