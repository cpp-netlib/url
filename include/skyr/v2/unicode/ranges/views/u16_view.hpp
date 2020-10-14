// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_RANGES_VIEWS_U16_VIEW_HPP
#define SKYR_V1_UNICODE_RANGES_VIEWS_U16_VIEW_HPP

#include <cassert>
#include <iterator>
#include <type_traits>
#include <tl/expected.hpp>
#include <skyr/v1/unicode/core.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <skyr/v1/unicode/code_points/u16.hpp>
#include <skyr/v1/unicode/ranges/sentinel.hpp>
#include <skyr/v1/unicode/traits/range_iterator.hpp>
#include <skyr/v1/unicode/traits/range_value.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \tparam U16Iterator
template <class U16Iterator>
class u16_range_iterator {

 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = tl::expected<u16_code_point_t, unicode_errc>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_pointer = const value_type *;
  ///
  using pointer = const_reference;
  ///
  using difference_type = std::ptrdiff_t;
  ///
  using size_type = std::size_t;

  ///
  /// \param first
  /// \param last
  explicit constexpr u16_range_iterator(
      U16Iterator first,
      U16Iterator last)
      : it_(first)
      , last_(last) {}

  ///
  /// \return
  constexpr auto operator ++ (int) noexcept -> u16_range_iterator {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  constexpr auto operator ++ () noexcept -> u16_range_iterator & {
    increment();
    return *this;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto operator * () const noexcept -> const_reference {
    assert(it_ != last_);

    auto value = mask16(static_cast<std::uint16_t>(*it_));
    if (is_lead_surrogate(value)) {
      auto next_it = it_;
      ++next_it;
      auto trail_value = mask16(static_cast<std::uint16_t>(*next_it));
      if (!is_trail_surrogate(trail_value)) {
        return tl::make_unexpected(unicode_errc::invalid_code_point);
      }

      return u16_code_point(static_cast<char16_t>(value), static_cast<char16_t>(trail_value));
    } else if (is_trail_surrogate(value)) {
      return tl::make_unexpected(unicode_errc::invalid_code_point);
    } else {
      return u16_code_point(static_cast<char16_t>(value));
    }
  }

  ///
  /// \param sentinel
  /// \return
  [[nodiscard]] constexpr auto operator == ([[maybe_unused]] sentinel sentinel) const noexcept {
    return it_ == last_;
  }

  ///
  /// \param sentinel
  /// \return
  [[nodiscard]] constexpr auto operator != ([[maybe_unused]] sentinel sentinel) const noexcept {
    return !(*this == sentinel);
  }

 private:

  constexpr void increment() {
    assert(it_ != last_);
    auto step = is_lead_surrogate(static_cast<char16_t>(mask16(static_cast<std::uint16_t>(*it_)))) ? 2u : 1u;
    std::advance(it_, step);
  }

  U16Iterator it_, last_;

};

///
/// \tparam U16Range
template <class U16Range>
class view_u16_range {

  using iterator_type = u16_range_iterator<traits::range_iterator_t<U16Range>>;

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
  explicit constexpr view_u16_range(U16Range range)
      : range_(std::move(range)) {}

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return iterator_type(std::cbegin(range_), std::cend(range_));
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
    return begin() == end();
  }

 private:

  U16Range range_;

};

namespace views {
///
///
/// \tparam U16Range
/// \param range
/// \return
template <typename U16Range>
constexpr inline auto as_u16(const U16Range &range) {
  static_assert(sizeof(traits::range_value_t<U16Range>) >= 2);
  return view_u16_range{range};
}
}  // namespace views
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_UNICODE_RANGES_VIEWS_U16_VIEW_HPP
