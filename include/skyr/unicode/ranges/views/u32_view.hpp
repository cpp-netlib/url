// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_U32_VIEW_HPP
#define SKYR_UNICODE_U32_VIEW_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <cassert>
#include <tl/expected.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \tparam U32Iterator
template <class U32Iterator>
class u32_range_iterator {

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
  u32_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr u32_range_iterator(
      U32Iterator it,
      U32Iterator last)
      : it_(it)
      , last_(last) {}

  ///
  /// \return
  u32_range_iterator operator ++ (int) noexcept {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  u32_range_iterator &operator ++ () noexcept {
    increment();
    return *this;
  }

  ///
  /// \return
  reference operator * () const noexcept {
    assert(it_);
    return u32_value(*it_.value());
  }

  ///
  /// \param other
  /// \return
  bool operator == (const u32_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  bool operator != (const u32_range_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    assert(it_);
    ++it_.value();
    if (it_ == last_) {
      it_ = std::nullopt;
    }
  }

  std::optional<U32Iterator> it_, last_;

};

///
/// \tparam U32Range
template <class U32Range>
class view_u32_range {

  using iterator_type = u32_range_iterator<typename traits::iterator<U32Range>::type>;

 public:

  ///
  using value_type = tl::expected<char32_t, std::error_code>;
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
  constexpr view_u32_range() noexcept = default;

  ///
  /// \param range
  explicit constexpr view_u32_range(const U32Range &range) noexcept
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

  const U32Range &range_;

};

///
struct u32_range_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename U32Range>
  constexpr auto operator()(
      U32Range &&range) const noexcept {
    return view_u32_range{std::forward<U32Range>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename U32Range>
  friend constexpr auto operator|(
      U32Range &&range,
      const u32_range_fn&) noexcept {
    return view_u32_range{std::forward<U32Range>(range)};
  }

};

namespace view {
///
static constexpr u32_range_fn as_u32;
}  // namespace view
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif //SKYR_UNICODE_U32_VIEW_HPP
