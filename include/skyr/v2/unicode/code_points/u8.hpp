// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_UNICODE_CODE_POINTS_U8_HPP
#define SKYR_V2_UNICODE_CODE_POINTS_U8_HPP

#include <skyr/v2/unicode/constants.hpp>
#include <skyr/v2/unicode/core.hpp>
#include <skyr/v2/unicode/errors.hpp>
#include <skyr/v2/unicode/traits/range_iterator.hpp>
#include <tl/expected.hpp>

namespace skyr::inline v2::unicode {
/// This class defines a view over a code point in raw bytes,
/// according to UTF-8.
/// \tparam OctetIterator An iterator type over the raw bytes
template <typename OctetIterator>
class u8_code_point_view {
 public:
  ///
  using const_iterator = OctetIterator;
  ///
  using iterator = const_iterator;
  ///
  using value_type = char;
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

  /// \brief Constructor
  /// \param first An iterator at the beginning of the code point
  /// \param last An iterator at the end of the code point
  constexpr u8_code_point_view(OctetIterator first, OctetIterator last) : first(first), last(last) {
  }

  /// \brief Constructor. The length of the code point sequence is
  ///        inferred from the first code point value.
  /// \param first An iterator at the beginning of the code point
  explicit constexpr u8_code_point_view(OctetIterator first)
      : u8_code_point_view(first, first + sequence_length(*first)) {
  }

  /// Returns an iterator to the beginning
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto begin() const noexcept {
    return first;
  }

  /// Returns an iterator to the end
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto end() const noexcept {
    return last;
  }

  /// Returns an iterator to the beginning
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return begin();
  }

  /// Returns an iterator to the end
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto cend() const noexcept {
    return end();
  }

  /// \brief Returns the length in bytes of this code point.
  /// \return
  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return sequence_length(*first);
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto u32_value() const noexcept {
    constexpr auto to_u32 = [](auto &&state) { return state.value; };
    return find_code_point(first).map(to_u32).value();
  }

 private:
  OctetIterator first, last;
};

///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
inline constexpr auto u8_code_point(const OctetRange &range)
    -> tl::expected<u8_code_point_view<traits::range_iterator_t<OctetRange>>, unicode_errc> {
  auto first = std::begin(range), last = std::end(range);
  auto length = sequence_length(*first);
  if (std::distance(first, last) > length) {
    return tl::make_unexpected(unicode_errc::overflow);
  }
  last = first;
  std::advance(last, length);
  return u8_code_point_view<traits::range_iterator_t<OctetRange>>(first, last);
}

///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
inline constexpr auto checked_u8_code_point(const OctetRange &range) {
  using result_type = tl::expected<u8_code_point_view<traits::range_iterator_t<OctetRange>>, unicode_errc>;

  constexpr auto check_code_point = [](auto &&code_point) -> result_type {
    return find_code_point(std::begin(code_point)).map([=](auto) { return code_point; });
  };

  return u8_code_point(range).and_then(check_code_point);
}
}  // namespace skyr::inline v2::unicode

#endif  // SKYR_V2_UNICODE_CODE_POINTS_U8_HPP
