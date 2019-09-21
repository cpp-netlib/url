// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_U16_RANGE_HPP
#define SKYR_U16_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <tl/expected.hpp>
#include <range/v3/view.hpp>
#include <skyr/unicode/unicode.hpp>
#include <skyr/unicode/range/octet_range.hpp>
#include <skyr/unicode/range/u32_range.hpp>

namespace skyr::unicode {
template <typename U16Iterator>
class u16_code_point_t {

 public:

  ///
  using const_iterator = U16Iterator;
  ///
  using iterator = const_iterator;
  ///
  using value_type = char16_t;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using size_type = std::size_t;

  ///
  /// \param first
  explicit constexpr u16_code_point_t(U16Iterator first)
  : first(first)
  , last(first + (*first > 0xffff? 2 : 1)) {}

  ///
  constexpr u16_code_point_t(const u16_code_point_t &) = default;
  ///
  constexpr u16_code_point_t(u16_code_point_t &&) noexcept = default;
  ///
  constexpr u16_code_point_t &operator=(const u16_code_point_t &) = default;
  ///
  constexpr u16_code_point_t &operator=(u16_code_point_t &&) noexcept = default;
  ///
  ~u16_code_point_t() = default;

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return first;
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return last;
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

  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return *first > 0xffff? 2 : 1;
  }

 private:

  U16Iterator first, last;

};

//template <typename OctetIterator>
//inline std::tuple<char16_t, char16_t> u16(u16_code_point_t<OctetIterator> code_point) {
//  auto state = find_code_point(begin(code_point));
//  if (state.value().value > 0xffff) {  // make a surrogate pair
//    return {
//      static_cast<char16_t>((state.value().value >> 10) +
//      constants::surrogates::lead_offset),
//      static_cast<char16_t>((state.value().value & 0x3ff) +
//      constants::surrogates::trail_min)
//    };
//  } else {
//    return {
//      static_cast<char16_t>(state.value().value),
//      0
//    };
//  }
//}

///
template <typename OctetIterator>
class u16_range_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = char16_t;
  ///
  using reference = value_type;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  u16_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr u16_range_iterator(unchecked_octet_range_iterator<OctetIterator> it)
      : it_(it) {}
  ///
  constexpr u16_range_iterator(const u16_range_iterator&) = default;
  ///
  constexpr u16_range_iterator(u16_range_iterator&&) noexcept = default;
  ///
  constexpr u16_range_iterator &operator=(const u16_range_iterator&) = default;
  ///
  constexpr u16_range_iterator &operator=(u16_range_iterator&&) noexcept = default;
  ///
  ~u16_range_iterator() = default;

  ///
  /// \return
  u16_range_iterator operator ++ (int) {
    auto result = *this;
    ++it_;
//    if (u32(*it_) > 0xffff) {
//      ++it_;
//    }
    return result;
  }

  ///
  /// \return
  u16_range_iterator &operator ++ () {
    ++it_;
//    if (u32(*it_) > 0xffff) {
//      ++it_;
//    }
    return *this;
  }

  ///
  /// \return
  reference operator * () const noexcept {
//    return u16(*it_);
    return 0;
  }

  ///
  /// \param other
  /// \return
  bool operator == (const u16_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  bool operator != (const u16_range_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  unchecked_octet_range_iterator<OctetIterator> it_;

};

///
/// \tparam OctetRange
template <typename OctetRange>
class view_u16_range
    : public ranges::view_base {

  using octet_iterator_type = typename OctetRange::const_iterator;
  using iterator_type = u16_range_iterator<octet_iterator_type>;

 public:

  ///
  using value_type = char16_t;
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
  constexpr view_u16_range() = default;

  ///
  /// \param range
  explicit constexpr view_u16_range(const OctetRange &range)
      : range_{range} {}

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return u16_range_iterator(range_.begin());
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return u16_range_iterator(range_.end());
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
    return range_.empty();
  }

  ///
  /// \return
  [[nodiscard]] constexpr size_type size() const noexcept {
    return range_.size();
  }

 private:

  view_unchecked_octet_range<OctetRange> range_;

};

///
struct u16_range_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(OctetRange &&range) const {
    return view_u16_range{std::forward<OctetRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(OctetRange &&range, const u16_range_fn&) {
    return view_u16_range{std::forward<OctetRange>(range)};
  }

};

namespace view {
///
static constexpr u16_range_fn u16;
}  // namespace view
}  // namespace skyr::unicode

#endif //SKYR_U16_RANGE_HPP
