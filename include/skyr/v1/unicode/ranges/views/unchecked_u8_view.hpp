// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_RANGES_VIEWS_UNCHECKED_U8_VIEW_HPP
#define SKYR_V1_UNICODE_RANGES_VIEWS_UNCHECKED_U8_VIEW_HPP

#include <iterator>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/v1/unicode/code_point.hpp>
#include <skyr/v1/unicode/core.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <skyr/v1/unicode/traits/range_iterator.hpp>
#include <skyr/v1/unicode/ranges/sentinel.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \tparam OctetIterator
template <
    class OctetIterator,
    class Sentinel=OctetIterator
    >
class unchecked_u8_range_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = u8_code_point_view<OctetIterator>;
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
  constexpr unchecked_u8_range_iterator() = default;
  ///
  /// \param it
  /// \param last
  constexpr unchecked_u8_range_iterator(OctetIterator it, Sentinel sentinel)
      : it_(it), sentinel_(sentinel) {}

  ///
  /// \return
  auto operator ++ (int) noexcept -> unchecked_u8_range_iterator {
    assert(it_);
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  auto operator ++ () noexcept -> unchecked_u8_range_iterator & {
    assert(it_);
    increment();
    return *this;
  }

  ///
  /// \return
  constexpr auto operator * () const noexcept -> const_reference {
    assert(it_);
    auto last = it_.value();
    std::advance(last, sequence_length(*it_.value()));
    return u8_code_point_view<OctetIterator>(it_.value(), last);
  }

  ///
  /// \param other
  /// \return
  constexpr auto operator == (const unchecked_u8_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr auto operator != (const unchecked_u8_range_iterator &other) const noexcept {
    return !(*this == other);
  }

  ///
  /// \param sentinel
  /// \return
  [[maybe_unused]] constexpr auto operator == ([[maybe_unused]] sentinel sentinel) const noexcept {
    return !it_;
  }

  ///
  /// \param sentinel
  /// \return
  [[maybe_unused]] constexpr auto operator != (sentinel sentinel) const noexcept {
    return !(*this == sentinel);
  }

 private:

  void increment() {
    std::advance(it_.value(), sequence_length(*it_.value()));
    if (it_ == sentinel_) {
      it_ = std::nullopt;
    }
  }

  std::optional<OctetIterator> it_;
  Sentinel sentinel_;

};

///
/// \tparam OctetRange
template <class OctetRange>
class view_unchecked_u8_range {

  using octet_iterator_type = traits::range_iterator_t<OctetRange>;
  using iterator_type = unchecked_u8_range_iterator<octet_iterator_type>;

 public:

  ///
  using value_type = u8_code_point_view<octet_iterator_type>;
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
  constexpr view_unchecked_u8_range() = default;

  ///
  /// \param range
  explicit constexpr view_unchecked_u8_range(
      const OctetRange &range)
      : impl_(
      impl(std::begin(range),
           std::end(range))) {}

  ///
  /// \return
  [[nodiscard]] constexpr auto begin() const noexcept {
    return impl_? impl_.value().first : iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto end() const noexcept {
    return impl_? impl_.value().last : iterator_type();
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
    return begin() == end();
  }

  ///
  /// \return
  [[nodiscard]] auto size() const noexcept {
    return static_cast<size_type>(std::distance(begin(), end()));
  }

 private:

  struct impl {
    constexpr impl(
        octet_iterator_type first,
        octet_iterator_type last)
        : first(first)
        , last(last) {}
    iterator_type first, last;
  };

  std::optional<impl> impl_;

};

namespace views {
///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
[[maybe_unused]] inline auto unchecked_u8(
    const OctetRange &range) {
  return view_unchecked_u8_range{range};
}
}  // namespace views
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_RANGES_VIEWS_UNCHECKED_U8_VIEW_HPP
