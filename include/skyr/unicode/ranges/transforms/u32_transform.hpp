// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_U32_RANGE_HPP
#define SKYR_UNICODE_U32_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>
#include <skyr/unicode/traits/range_iterator.hpp>
#include <skyr/unicode/ranges/views/u8_view.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \tparam CodePointIterator
template <class CodePointIterator>
class transform_u32_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = tl::expected<char32_t, std::error_code>;
  ///
  using reference = value_type;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  constexpr transform_u32_iterator() = default;
  ///
  /// \param it
  explicit constexpr transform_u32_iterator(CodePointIterator it)
      : it_(it) {}

  ///
  /// \return
  transform_u32_iterator operator ++ (int) noexcept {
    auto result = *this;
    ++it_;
    return result;
  }

  ///
  /// \return
  transform_u32_iterator &operator ++ () noexcept {
    ++it_;
    return *this;
  }

  ///
  /// \return
  [[nodiscard]] reference operator*() const noexcept {
    return (*it_).and_then([](auto code_point) { return u32_value(code_point); });
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator == (const transform_u32_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator != (const transform_u32_iterator &other) const noexcept {
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
  constexpr transform_u32_range() noexcept = default;

  ///
  /// \param range
  explicit constexpr transform_u32_range(CodePointRange &&range) noexcept
      : range_(std::forward<CodePointRange>(range)) {}

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return const_iterator(range_.begin());
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
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
  [[nodiscard]] constexpr bool empty() const noexcept {
    return range_.empty();
  }

  ///
  /// \return
  [[nodiscard]] constexpr size_type size() const noexcept {
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
      CodePointRange &&range) const noexcept {
    return transform_u32_range{std::forward<CodePointRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <class CodePointRange>
  friend constexpr auto operator|(
      CodePointRange &&range,
      const transform_u32_range_fn&) noexcept {
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
tl::expected<Output, std::error_code> as(transform_u32_range<CodePointRange> &&range) {
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

#endif //SKYR_UNICODE_U32_RANGE_HPP
