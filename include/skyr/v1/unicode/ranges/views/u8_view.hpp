// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_RANGES_VIEWS_U8_VIEW_HPP
#define SKYR_V1_UNICODE_RANGES_VIEWS_U8_VIEW_HPP

#include <iterator>
#include <type_traits>
#include <tl/expected.hpp>
#include <skyr/v1/unicode/code_point.hpp>
#include <skyr/v1/unicode/core.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <skyr/v1/unicode/ranges/views/unchecked_u8_view.hpp>
#include <skyr/v1/unicode/traits/iterator_value.hpp>
#include <skyr/v1/unicode/traits/range_iterator.hpp>
#include <skyr/v1/unicode/traits/range_value.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
/// \brief
///
/// \tparam OctetIterator
template <class OctetIterator>
class u8_range_iterator {

  using iterator_type = unchecked_u8_range_iterator<OctetIterator>;

 public:

  /// \c std::forward_iterator_tag
  using iterator_category = std::forward_iterator_tag;
  /// An expected value containing a UTF-8 encoded code point
  using value_type = tl::expected<traits::iterator_value_t<iterator_type>, unicode_errc>;
  /// A reference type
  using const_reference = value_type;
  /// A reference type
  using reference = const_reference;
  /// A pointer type
  using const_pointer = const value_type *;
  /// A pointer type
  using pointer = const_pointer;
  /// \c std::ptrdiff_t
  using difference_type = std::ptrdiff_t;
  /// \c std::size_t
  using size_type = std::size_t;

  /// \brief Constructs a \c u8_range_iterator from a range of octets
  ///
  /// \param first
  /// \param last
  constexpr u8_range_iterator(
      OctetIterator first,
      OctetIterator last)
      : it_(iterator_type(first, last)) {}

  /// \brief Post-increment operator
  ///
  /// Increments through a code point
  ///
  /// \return The previous iterator value
  constexpr auto operator ++ (int) noexcept -> u8_range_iterator {
    auto result = *this;
    ++it_;
    return result;
  }

  /// \brief Pre-increment operator
  ///
  /// Increments through a code point
  ///
  /// \return \c *this
  constexpr auto operator ++ () noexcept -> u8_range_iterator & {
    ++it_;
    return *this;
  }

  /// \brief Dereference operator
  ///
  /// Returns a proxy to a UTF-8 encoded code point
  ///
  /// \return A proxy to a UTF-8 encoded code point
  constexpr auto operator * () const noexcept -> const_reference {
    return checked_u8_code_point(*it_);
  }

  ///
  /// \param sentinel
  /// \return
  [[maybe_unused]] constexpr auto operator == (sentinel sentinel) const noexcept {
    return it_ == sentinel;
  }

  ///
  /// \param sentinel
  /// \return
  [[maybe_unused]] constexpr auto operator != (sentinel sentinel) const noexcept {
    return !(*this == sentinel);
  }

 private:

  iterator_type it_;

};

///
/// \tparam OctetRange
template <class OctetRange>
class view_u8_range {

  using octet_iterator_type = traits::range_iterator_t<OctetRange>;
  using iterator_type = u8_range_iterator<octet_iterator_type>;

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
  /// \c std::size_t
  using size_type = std::size_t;

  ///
  /// \param range
  explicit constexpr view_u8_range(const OctetRange &range)
      : it_(std::cbegin(range), std::cend(range)) {}

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return it_;
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

  iterator_type it_;

};

namespace views {
///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
constexpr inline auto as_u8(const OctetRange &range) {
  static_assert(sizeof(traits::range_value_t<OctetRange>) >= 1);
  return view_u8_range{range};
}
}  // namespace views
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_RANGES_VIEWS_U8_VIEW_HPP
