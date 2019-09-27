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
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>
#include <skyr/unicode/range/u8_range.hpp>
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

///
/// \param code_point
/// \return
inline u16_code_point_t u16_code_point(char32_t code_point) {
  return u16_code_point_t(code_point);
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline u16_code_point_t u16(u8_code_point_t<OctetIterator> code_point) {
  return u16_code_point(details::u32(code_point));
}

///
/// \tparam OctetIterator
template <typename OctetIterator>
class u16_range_iterator {

 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = tl::expected<u16_code_point_t, std::error_code>;
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
      u32_range_iterator<OctetIterator> it,
      u32_range_iterator<OctetIterator> last)
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
    auto result = *this;
    ++it_;
    return result;
  }

  ///
  /// \return
  u16_range_iterator &operator ++ () {
    ++it_;
    return *this;
  }

  ///
  /// \return
  reference operator * () const noexcept {
    auto code_point = *it_;
    return
    code_point
    .and_then([] (auto value) -> value_type {
      return u16_code_point(value);
    });
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

  u32_range_iterator<OctetIterator> it_, last_;

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
  using value_type = tl::expected<u16_code_point_t, std::error_code>;
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

  view_u32_range<OctetRange> range_;

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

template <typename U16Range>
tl::expected<std::u16string, std::error_code> u16string(U16Range &&range) {
  auto result = std::u16string();
  result.reserve(ranges::size(range));
  for (auto &&code_point : range) {
    if (!code_point) {
      return tl::make_unexpected(code_point.error());
    }
    result.push_back(code_point.value().lead_value());
    if (code_point.value().is_surrogate_pair()) {
      result.push_back(code_point.value().trail_value());
    }
  }
  return result;
}

template <typename U16Range>
tl::expected<std::wstring, std::error_code> wstring(U16Range &&range) {
  auto result = std::wstring();
  result.reserve(ranges::size(range));
  for (auto &&code_point : range) {
    if (!code_point) {
      return tl::make_unexpected(code_point.error());
    }
    result.push_back(code_point.value().lead_value());
    if (code_point.value().is_surrogate_pair()) {
      result.push_back(code_point.value().trail_value());
    }
  }
  return result;
}
}  // namespace skyr::unicode

#endif //SKYR_U16_RANGE_HPP
