// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_RANGES_TRANSFORMS_U32_TRANSFORM_HPP
#define SKYR_V1_UNICODE_RANGES_TRANSFORMS_U32_TRANSFORM_HPP

#include <iterator>
#include <optional>
#include <skyr/v1/unicode/core.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <skyr/v1/unicode/ranges/views/u8_view.hpp>
#include <skyr/v1/unicode/traits/range_iterator.hpp>
#include <tl/expected.hpp>
#include <type_traits>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \tparam CodePointIterator
template <class CodePointIterator>
class u32_transform_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = tl::expected<char32_t, std::error_code>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_pointer = const typename std::add_pointer<value_type>::type;
  ///
  using pointer = const_pointer;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  constexpr u32_transform_iterator() = default;
  ///
  /// \param it
  explicit constexpr u32_transform_iterator(CodePointIterator it)
      : it_(it) {}

  ///
  /// \return
  auto operator ++ (int) noexcept {
    auto result = *this;
    ++it_;
    return result;
  }

  ///
  /// \return
  auto &operator ++ () noexcept {
    ++it_;
    return *this;
  }

  ///
  /// \return
  [[nodiscard]] auto operator*() const noexcept -> const_reference {
    constexpr static auto to_u32 = [](auto code_point) { return u32_value(code_point); };
    return (*it_).and_then(to_u32);
  }

  ///
  /// \param other
  /// \return
  constexpr auto operator == (const u32_transform_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr auto operator != (const u32_transform_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  CodePointIterator it_;

};

///
/// \tparam OctetRange
template <class CodePointRange>
class transform_u32_range {

  using iterator_type = typename traits::range_iterator<CodePointRange>::type;

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
  constexpr transform_u32_range() = default;

  ///
  /// \param range
  explicit constexpr transform_u32_range(CodePointRange &&range)
      : range_(std::forward<CodePointRange>(range)) {}

  ///
  /// \return
  [[nodiscard]] constexpr auto begin() const noexcept {
    return const_iterator(range_.begin());
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto end() const noexcept {
    return const_iterator(range_.end());
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
  [[nodiscard]] constexpr auto empty() const noexcept {
    return range_.empty();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return range_.size();
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
  constexpr auto operator()(
      CodePointRange &&range) const {
    return transform_u32_range{std::forward<CodePointRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <class CodePointRange>
  friend constexpr auto operator|(
      CodePointRange &&range,
      const transform_u32_range_fn&) {
    return transform_u32_range{std::forward<CodePointRange>(range)};
  }

};

namespace transform {
///
static constexpr transform_u32_range_fn to_u32;
}  // namespace transform

///
/// \tparam Output
/// \tparam CodePointRange
/// \param range
/// \return
template <class Output, class CodePointRange>
auto as(transform_u32_range<CodePointRange> &&range) -> tl::expected<Output, std::error_code> {
  auto result = Output{};
  for (auto &&code_point : range) {
    auto u32_code_point = u32_value(code_point);
    if (!u32_code_point) {
      return tl::make_unexpected(u32_code_point.error());
    }
    result.push_back(u32_code_point.value());
  }
  return result;
}
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_RANGES_TRANSFORMS_U32_TRANSFORM_HPP
