// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_U16_VIEW_HPP
#define SKYR_UNICODE_U16_VIEW_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <cassert>
#include <tl/expected.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>
#include <skyr/unicode/traits/range_iterator.hpp>
#include <skyr/unicode/traits/range_value.hpp>
#include <skyr/unicode/ranges/views/u8_view.hpp>
#include <skyr/unicode/ranges/transforms/u32_transform.hpp>

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
      U16Iterator it,
      U16Iterator last)
      : it_(it)
      , last_(last) {}

  ///
  /// \return
  u16_range_iterator operator ++ (int) noexcept {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  u16_range_iterator &operator ++ () noexcept {
    increment();
    return *this;
  }

  ///
  /// \return
  reference operator * () const noexcept {
    assert(it_);
    auto value = mask16(*it_.value());
    if (is_lead_surrogate(value)) {
      auto next_it = it_.value();
      ++next_it;
      auto trail_value = mask16(*next_it);
      if (!is_trail_surrogate(trail_value)) {
        return tl::make_unexpected(
            make_error_code(unicode_errc::invalid_code_point));
      }

      return u16_code_point(value, trail_value);
    } else if (is_trail_surrogate(value)) {
      return tl::make_unexpected(
          make_error_code(unicode_errc::invalid_code_point));
    } else {
      return u16_code_point(value);
    }
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

  void increment() {
    assert(it_);
    auto value = mask16(*it_.value());
    std::advance(it_.value(), is_lead_surrogate(value)? 2 : 1);
    if (it_ == last_) {
      it_ = std::nullopt;
    }
  }

  std::optional<U16Iterator> it_, last_;

};

///
/// \tparam U16Range
template <class U16Range>
class view_u16_range {

  using iterator_type = u16_range_iterator<typename traits::range_iterator<U16Range>::type>;

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
  constexpr view_u16_range() noexcept = default;

  ///
  /// \param range
  explicit constexpr view_u16_range(const U16Range &range) noexcept
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
    return begin() == end();
  }

  ///
  /// \return
  [[nodiscard]] constexpr size_type size() const noexcept {
    return std::distance(begin(), end());
  }

 private:

  U16Range range_;

};

namespace view {
///
///
/// \tparam U16Range
/// \param range
/// \return
template <typename U16Range>
inline auto as_u16(const U16Range &range) noexcept {
  static_assert(sizeof(typename traits::range_value<U16Range>::type) >= 1);
  return view_u16_range{range};
}
}  // namespace view
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif //SKYR_UNICODE_U16_VIEW_HPP
