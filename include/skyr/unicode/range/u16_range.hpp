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
class u16_code_point_t {

 public:

  ///
  /// \param first
  explicit constexpr u16_code_point_t(char32_t code_point)
  : code_point_(code_point) {}

  ///
  constexpr u16_code_point_t(const u16_code_point_t &) = default;
  ///
  constexpr u16_code_point_t(u16_code_point_t &&) noexcept = default;
  ///
  u16_code_point_t &operator=(const u16_code_point_t &) = default;
  ///
  u16_code_point_t &operator=(u16_code_point_t &&) noexcept = default;
  ///
  ~u16_code_point_t() = default;

  [[nodiscard]] uint16_t lead_value() const {
    return is_surrogate_pair()?
    static_cast<char16_t>((code_point_ >> 10U) + constants::surrogates::lead_offset) :
    static_cast<char16_t>(code_point_);
  }

  [[nodiscard]] uint16_t trail_value() const {
    return is_surrogate_pair()?
    static_cast<char16_t>((code_point_ & 0x3ffU) + constants::surrogates::trail_min) :
    0;
  }

  [[nodiscard]] constexpr bool is_surrogate_pair() const noexcept {
    return code_point_ > 0xffffU;
  }

 private:

  char32_t code_point_;

};

inline u16_code_point_t u16_code_point(char32_t code_point) {
  return u16_code_point_t(code_point);
}

///
template <typename OctetIterator>
class u16_range_iterator {

  using iterator_type = u32_range_iterator<OctetIterator>;

 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = u16_code_point_t;
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
  explicit constexpr u16_range_iterator(
      octet_range_iterator<OctetIterator> it,
      octet_range_iterator<OctetIterator> last)
      : it_(it)
      , last_(last) {}
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
    assert(it_);
    auto result = *this;
    ++it_.value();
    return result;
  }

  ///
  /// \return
  u16_range_iterator &operator ++ () {
    assert(it_);
    ++it_.value();
    return *this;
  }

  ///
  /// \return
  reference operator * () const noexcept {
    return u16_code_point(*it_.value());
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

  std::optional<octet_range_iterator<OctetIterator>> it_, last_;

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
    return iterator_type(std::begin(range_), std::end(range_));
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
    return range_.empty();
  }

  ///
  /// \return
  [[nodiscard]] constexpr size_type size() const noexcept {
    return range_.size();
  }

 private:

  view_octet_range<OctetRange> range_;

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
