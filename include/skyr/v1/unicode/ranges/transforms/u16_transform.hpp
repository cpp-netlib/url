// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_RANGES_TRANSFORMS_U16_TRANSFORM_HPP
#define SKYR_V1_UNICODE_RANGES_TRANSFORMS_U16_TRANSFORM_HPP

#include <iterator>
#include <optional>
#include <skyr/v1/unicode/core.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <skyr/v1/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/v1/unicode/ranges/views/u8_view.hpp>
#include <skyr/v1/unicode/traits/range_iterator.hpp>
#include <tl/expected.hpp>
#include <type_traits>

namespace skyr {
inline namespace v1 {
namespace unicode {
/// An iterator that transform a code point to UTF-16 code
/// units
///
/// \tparam CodePointIterator
template <class CodePointIterator>
class u16_transform_iterator {

 public:

  /// \c std::forward_iterator_tag
  using iterator_category = std::forward_iterator_tag;
  /// An expected wrapper around a UTF-16 encoded code point
  using value_type = tl::expected<u16_code_point_t, unicode_errc>;
  /// \c value_type
  using const_reference = value_type;
  /// \c const_reference
  using reference = const_reference;
  /// \c value_type *
  using const_pointer = const typename std::add_pointer<value_type>::type;
  /// \c value_type *
  using pointer = const_pointer;
  /// \c std::ptrdiff_t
  using difference_type = std::ptrdiff_t;
  /// \c std::size_t
  using size_type = std::size_t;

  /// Default constructor
  u16_transform_iterator() = default;
  ///
  /// \param first
  /// \param last
  explicit constexpr u16_transform_iterator(
      CodePointIterator first,
      CodePointIterator last)
      : it_(first)
      , last_(last) {}

  /// Pre-increment operator
  /// \return A reference to this iterator
  auto operator ++ () noexcept -> u16_transform_iterator & {
    ++it_;
    return *this;
  }

  /// Post-increment operator
  /// \return A copy of the previous iterator
  auto operator ++ (int) noexcept -> u16_transform_iterator {
    auto result = *this;
    ++it_;
    return result;
  }

  /// Dereference operator
  /// \return An expected value
  [[nodiscard]] auto operator * () const noexcept -> const_reference {
    constexpr static auto to_u16 = [](auto value) { return u16_code_point(value); };
    auto code_point = *it_;
    return code_point.map(to_u16);
  }

  /// Equality operator
  /// \param other The other iterator
  /// \return \c true if the iterators are the same, \c false otherwise
  auto operator == (const u16_transform_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  /// Inequality operator
  /// \param other The other iterator
  /// \return \c true if the iterators are not the same, \c false otherwise
  bool operator != (const u16_transform_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  u32_transform_iterator<CodePointIterator> it_, last_;

};

/// A range that transforms code point values to a UTF-16 sequence
///
/// \tparam CodePointRange
template <class CodePointRange>
class transform_u16_range {

  using iterator_type =
      u16_transform_iterator<typename traits::range_iterator<CodePointRange>::type>;

 public:

  ///
  using value_type = tl::expected<u16_code_point_t, unicode_errc>;
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
  explicit constexpr transform_u16_range(
      CodePointRange &&range)
      : range_{std::forward<CodePointRange>(range)} {}

  /// Returns an iterator to the beginning
  /// \return \c const_iterator
  [[nodiscard]] auto begin() const noexcept {
    return iterator_type(std::begin(range_), std::end(range_));
  }

  /// Returns an iterator to the end
  /// \return \c const_iterator
  [[nodiscard]] auto end() const noexcept {
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
  [[nodiscard]] constexpr auto empty() const noexcept {
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
  constexpr auto operator()(
      CodePointRange &&range) const {
    return transform_u16_range{std::forward<CodePointRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <class CodePointRange>
  friend constexpr auto operator|(
      CodePointRange &&range,
      const transform_u16_range_fn&) {
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
auto as(transform_u16_range<CodePointRange> &&range) -> tl::expected<Output, unicode_errc> {
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
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_RANGES_TRANSFORMS_U16_TRANSFORM_HPP
