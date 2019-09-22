// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_U32_RANGE_HPP
#define SKYR_U32_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <tl/expected.hpp>
#include <range/v3/view.hpp>
#include <skyr/unicode/unicode.hpp>
#include <skyr/unicode/range/octet_range.hpp>

namespace skyr::unicode {
///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline char32_t u32(code_point_octet_t<OctetIterator> code_point) {
  auto state = find_code_point(std::begin(code_point));
  return state ? state.value().value : U'\x0000';
}

///
/// \tparam OctetIterator
template <typename OctetIterator>
class u32_range_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = tl::expected<char32_t, unicode_errc>;
  ///
  using reference = value_type;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  constexpr u32_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr u32_range_iterator(octet_range_iterator<OctetIterator> it)
      : it_(it) {}
  ///
  constexpr u32_range_iterator(const u32_range_iterator&) = default;
  ///
  constexpr u32_range_iterator(u32_range_iterator&&) noexcept = default;
  ///
  constexpr u32_range_iterator &operator=(const u32_range_iterator&) = default;
  ///
  constexpr u32_range_iterator &operator=(u32_range_iterator&&) noexcept = default;
  ///
  ~u32_range_iterator() = default;

  ///
  /// \return
  u32_range_iterator operator ++ (int) {
    auto result = *this;
    ++it_;
    return result;
  }

  ///
  /// \return
  u32_range_iterator &operator ++ () {
    ++it_;
    return *this;
  }

  ///
  /// \return
  reference operator * () const noexcept {
    return (*it_)
    .and_then([] (auto code_point) -> value_type {
      return u32(code_point);
    });
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator == (const u32_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator != (const u32_range_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  octet_range_iterator<OctetIterator> it_;

};

///
/// \tparam OctetRange
template <typename OctetRange>
class view_u32_range
    : public ranges::view_base {

  using octet_iterator_type = typename OctetRange::const_iterator;
  using iterator_type = u32_range_iterator<octet_iterator_type>;

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
  constexpr view_u32_range() = default;

  ///
  /// \param range
  explicit constexpr view_u32_range(const OctetRange &range)
      : range_{range} {}

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return u32_range_iterator(range_.begin());
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return u32_range_iterator(range_.end());
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

  view_octet_range<OctetRange> range_;

};

///
struct u32_range_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(OctetRange &&range) const {
    return view_u32_range{std::forward<OctetRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(OctetRange &&range, const u32_range_fn&) {
    return view_u32_range{std::forward<OctetRange>(range)};
  }

};

namespace view {
///
static constexpr u32_range_fn u32;
}  // namespace view

template <typename U32Range>
tl::expected<std::u32string, unicode_errc> u32string(U32Range &&range) {
  auto result = std::u32string();
  result.reserve(ranges::size(range));
  for (auto &&code_point : range) {
    if (!code_point) {
      return tl::make_unexpected(code_point.error());
    }
    result.push_back(code_point.value());
  }
  return result;
}
}  // namespace skyr::unicode

#endif //SKYR_U32_RANGE_HPP
