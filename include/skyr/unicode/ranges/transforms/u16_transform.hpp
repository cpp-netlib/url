// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_U16_RANGE_HPP
#define SKYR_UNICODE_U16_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>
#include <skyr/unicode/ranges/traits.hpp>
#include <skyr/unicode/ranges/views/u8_view.hpp>
#include <skyr/unicode/ranges/transforms/u32_transform.hpp>

namespace skyr::unicode {
///
/// \tparam CodePointIterator
template <class CodePointIterator>
class transform_u16_iterator {

 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = tl::expected<u16_code_point_t, std::error_code>;
  ///
  using reference = value_type;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  /// \c std::ptrdiff_t
  using difference_type = std::ptrdiff_t;

  /// Default constructor
  transform_u16_iterator() = default;
  ///
  /// \param it
  explicit constexpr transform_u16_iterator(
      CodePointIterator it,
      CodePointIterator last)
      : it_(it)
      , last_(last) {}
  ///
  constexpr transform_u16_iterator(const transform_u16_iterator&) = default;
  ///
  constexpr transform_u16_iterator(transform_u16_iterator&&) noexcept = default;
  ///
  constexpr transform_u16_iterator &operator=(const transform_u16_iterator&) = default;
  ///
  constexpr transform_u16_iterator &operator=(transform_u16_iterator&&) noexcept = default;
  /// Destructor
  ~transform_u16_iterator() = default;

  /// Pre-increment operator
  /// \return A reference to this iterator
  transform_u16_iterator &operator ++ () {
    ++it_;
    return *this;
  }

  /// Post-increment operator
  /// \return A copy of the previous iterator
  transform_u16_iterator operator ++ (int) {
    auto result = *this;
    ++it_;
    return result;
  }

  /// Dereference operator
  /// \return An expected value
  reference operator * () const noexcept {
    auto code_point = *it_;
    return
    code_point
    .and_then([] (auto value) -> value_type {
      return u16_code_point(value);
    });
  }

  /// Equality operator
  /// \param other The other iterator
  /// \return \c true if the iterators are the same, \c false otherwise
  bool operator == (const transform_u16_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  /// Inequality operator
  /// \param other The other iterator
  /// \return \c true if the iterators are not the same, \c false otherwise
  bool operator != (const transform_u16_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  transform_u32_iterator<CodePointIterator> it_, last_;

};

///
/// \tparam CodePointRange
template <class CodePointRange>
class transform_u16_range {

  using iterator_type = transform_u16_iterator<typename traits::iterator<CodePointRange>::type>;

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

  /// Default constructor
  constexpr transform_u16_range() = default;

  ///
  /// \param range
  explicit constexpr transform_u16_range(CodePointRange &&range)
      : range_{std::forward<CodePointRange>(range)} {}

  /// Returns an iterator to the beginning
  /// \return \c const_iterator
  [[nodiscard]] const_iterator begin() const noexcept {
    return iterator_type(std::begin(range_), std::end(range_));
  }

  /// Returns an iterator to the end
  /// \return \c const_iterator
  [[nodiscard]] const_iterator end() const noexcept {
    return iterator_type();
  }

  /// Returns an iterator to the beginning
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return begin();
  }

  /// Returns an iterator to the end
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto cend() const noexcept {
    return end();
  }

  /// Tests if the byte range is empty
  /// \return \c true if the range is empty, \c false otherwise
  [[nodiscard]] constexpr bool empty() const noexcept {
    return range_.empty();
  }
//
//  ///
//  /// \return
//  [[nodiscard]] constexpr size_type size() const noexcept {
//    return range_.size();
//  }

 private:

  transform_u32_range<CodePointRange> range_;

};

///
struct transform_u16_range_fn {
  ///
  /// \tparam CodePointRange
  /// \param range
  /// \return
  template <class CodePointRange>
  constexpr auto operator()(CodePointRange &&range) const {
    return transform_u16_range{std::forward<CodePointRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <class CodePointRange>
  friend constexpr auto operator|(CodePointRange &&range, const transform_u16_range_fn&) {
    return transform_u16_range{std::forward<CodePointRange>(range)};
  }

};

namespace transform {
///
static constexpr transform_u16_range_fn to_u16;
}  // namespace transform

/// A sink that converts a U16 range to  string.
/// \tparam Output
/// \tparam OctetRange
/// \param range
/// \return
template <class Output, class CodePointRange>
tl::expected<Output, std::error_code> as(transform_u16_range<CodePointRange> &&range) {
  auto result = Output{};
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

#endif //SKYR_UNICODE_U16_RANGE_HPP
