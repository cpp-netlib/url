// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_RANGES_TRANSFORMS_U16_TRANSFORM_HPP
#define SKYR_V1_UNICODE_RANGES_TRANSFORMS_U16_TRANSFORM_HPP

#include <iterator>
#include <type_traits>
#include <tl/expected.hpp>
#include <skyr/v1/unicode/core.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <skyr/v1/unicode/code_points/u16.hpp>
#include <skyr/v1/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/v1/unicode/ranges/sentinel.hpp>
#include <skyr/v1/unicode/traits/range_iterator.hpp>

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
  using const_pointer = const value_type *;
  /// \c value_type *
  using pointer = const_pointer;
  /// \c std::ptrdiff_t
  using difference_type = std::ptrdiff_t;
  /// \c std::size_t
  using size_type = std::size_t;

  ///
  /// \param first
  /// \param last
  explicit constexpr u16_transform_iterator(
      CodePointIterator it)
      : it_(it) {}

  /// Pre-increment operator
  /// \return A reference to this iterator
  constexpr auto operator ++ () noexcept -> u16_transform_iterator & {
    ++it_;
    return *this;
  }

  /// Post-increment operator
  /// \return A copy of the previous iterator
  constexpr auto operator ++ (int) noexcept -> u16_transform_iterator {
    auto result = *this;
    ++it_;
    return result;
  }

  /// Dereference operator
  /// \return An expected value
  [[nodiscard]] constexpr auto operator * () const noexcept -> const_reference {
    constexpr auto to_u16 = [](auto value) { return u16_code_point(value); };
    auto code_point = *it_;
    return code_point.map(to_u16);
  }

  [[nodiscard]] constexpr auto operator == (sentinel sentinel) const noexcept {
    return it_ == sentinel;
  }

  [[nodiscard]] constexpr auto operator != (sentinel sentinel) const noexcept {
    return !(*this == sentinel);
  }

 private:

  u32_transform_iterator<CodePointIterator> it_;

};

/// A range that transforms code point values to a UTF-16 sequence
///
/// \tparam CodePointRange
template <class CodePointRange>
class transform_u16_range {

  using iterator_type =
      u16_transform_iterator<traits::range_iterator_t<CodePointRange>>;

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

  ///
  /// \param range
  explicit constexpr transform_u16_range(
      CodePointRange &&range)
      : range_{std::forward<CodePointRange>(range)} {}

  /// Returns an iterator to the beginning
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return const_iterator(std::cbegin(range_));
  }

  /// Returns an iterator to the end
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto cend() const noexcept {
    return sentinel{};
  }

  /// Returns an iterator to the beginning
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto begin() const noexcept {
    return cbegin();
  }

  /// Returns an iterator to the end
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto end() const noexcept {
    return cend();
  }

  /// Tests if the byte range is empty
  /// \return \c true if the range is empty, \c false otherwise
  [[nodiscard]] constexpr auto empty() const noexcept {
    return range_.empty();
  }

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

namespace transforms {
///
static constexpr transform_u16_range_fn to_u16;
}  // namespace transforms

/// A sink that converts a U16 range to  string.
/// \tparam Output
/// \tparam OctetRange
/// \param range
/// \return
template <class Output, class CodePointRange>
auto as(transform_u16_range<CodePointRange> &&range) -> tl::expected<Output, unicode_errc> {
  auto result = Output{};

  for (auto it = std::cbegin(range); it != std::cend(range); ++it) {
    auto code_point = *it;
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
