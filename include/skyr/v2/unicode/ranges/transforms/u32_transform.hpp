// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_UNICODE_RANGES_TRANSFORMS_U32_TRANSFORM_HPP
#define SKYR_V2_UNICODE_RANGES_TRANSFORMS_U32_TRANSFORM_HPP

#include <iterator>
#include <type_traits>
#include <tl/expected.hpp>
#include <skyr/v2/unicode/core.hpp>
#include <skyr/v2/unicode/errors.hpp>
#include <skyr/v2/unicode/ranges/sentinel.hpp>
#include <skyr/v2/unicode/traits/range_iterator.hpp>

namespace skyr::inline v2::unicode {
///
/// \tparam CodePointIterator
template <class CodePointIterator>
class u32_transform_iterator {
 public:
  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = tl::expected<char32_t, unicode_errc>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_pointer = const value_type *;
  ///
  using pointer = const_pointer;
  ///
  using difference_type = std::ptrdiff_t;
  ///
  using size_type = std::size_t;

  ///
  /// \param it
  explicit constexpr u32_transform_iterator(CodePointIterator it) : it_(it) {
  }

  ///
  /// \return
  constexpr auto operator++(int) noexcept -> u32_transform_iterator {
    auto result = *this;
    ++it_;
    return result;
  }

  ///
  /// \return
  constexpr auto operator++() noexcept -> u32_transform_iterator & {
    ++it_;
    return *this;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto operator*() const noexcept -> const_reference {
    constexpr auto to_u32 = [](auto code_point) { return u32_value(code_point); };
    return (*it_).and_then(to_u32);
  }

  [[nodiscard]] constexpr auto operator==(sentinel sentinel) const noexcept {
    return (it_ == sentinel);
  }

  [[nodiscard]] constexpr auto operator!=(sentinel sentinel) const noexcept {
    return !(*this == sentinel);
  }

 private:
  CodePointIterator it_;
};

///
/// \tparam OctetRange
template <class CodePointRange>
class transform_u32_range {
  using iterator_type = traits::range_iterator_t<CodePointRange>;

 public:
  ///
  using value_type = char32_t;
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
  explicit constexpr transform_u32_range(CodePointRange &&range) : range_(std::forward<CodePointRange>(range)) {
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return const_iterator(std::cbegin(range_));
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
    return range_.empty();
  }

 private:
  CodePointRange range_;
};

///
struct transform_u32_range_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <class CodePointRange>
  constexpr auto operator()(CodePointRange &&range) const {
    return transform_u32_range{std::forward<CodePointRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <class CodePointRange>
  friend constexpr auto operator|(CodePointRange &&range, const transform_u32_range_fn &) {
    return transform_u32_range{std::forward<CodePointRange>(range)};
  }
};

namespace transforms {
///
static constexpr transform_u32_range_fn to_u32;
}  // namespace transforms

///
/// \tparam Output
/// \tparam CodePointRange
/// \param range
/// \return
template <class Output, class CodePointRange>
constexpr auto as(transform_u32_range<CodePointRange> &&range) -> tl::expected<Output, unicode_errc> {
  auto result = Output{};

  for (auto it = std::cbegin(range); it != std::cend(range); ++it) {
    auto code_point = *it;
    auto u32_code_point = u32_value(code_point);
    if (!u32_code_point) {
      return tl::make_unexpected(u32_code_point.error());
    }
    result.push_back(u32_code_point.value());
  }

  return result;
}
}  // namespace skyr::inline v2::unicode

#endif  // SKYR_V2_UNICODE_RANGES_TRANSFORMS_U32_TRANSFORM_HPP
