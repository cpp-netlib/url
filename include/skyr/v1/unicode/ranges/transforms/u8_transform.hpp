// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_RANGES_TRANSFORMS_U8_TRANSFORM_HPP
#define SKYR_V1_UNICODE_RANGES_TRANSFORMS_U8_TRANSFORM_HPP

#include <iterator>
#include <tl/expected.hpp>
#include <skyr/v1/unicode/constants.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <skyr/v1/unicode/code_point.hpp>
#include <skyr/v1/unicode/ranges/sentinel.hpp>
#include <skyr/v1/unicode/traits/range_iterator.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
/// An iterator that transforms a code point to bytes
/// (as UTF-8) when dereferenced
///
/// \tparam CodePointIterator
template<class CodePointIterator, class Sentinel>
class u8_transform_iterator {

 public:

  /// \c std::forward_iterator_tag
  using iterator_category = std::forward_iterator_tag;
  /// An expected wrapper around a \c char
  using value_type = tl::expected<char, unicode_errc>;
  /// A reference
  using const_reference = value_type;
  /// A reference
  using reference = const_reference;
  /// A pointer
  using const_pointer = const value_type *;
  /// A pointer
  using pointer = const_pointer;
  /// \c std::ptrdiff_t
  using difference_type = std::ptrdiff_t;
  /// \c std::size_t
  using size_type = std::size_t;

  /// Constructs an iterator from an iterator that iterates over
  /// code points
  ///
  /// \param first The first iterator in the code point sequence
  /// \param last The end iterator in the code point sequence
  constexpr u8_transform_iterator(
      CodePointIterator first,
      Sentinel last)
      : it_(first), last_(last) {}

  /// Pre-increment operator
  /// \return A reference to this iterator
  auto operator++() noexcept -> u8_transform_iterator & {
    increment();
    return *this;
  }

  /// Post-increment operator
  /// \return A copy of the previous iterator
  constexpr auto operator++(int) noexcept -> u8_transform_iterator {
    auto result = *this;
    increment();
    return result;
  }

  /// Dereference operator
  ///
  /// Returns the value of the octet as if iterating through a
  /// UTF-8 encoded sequence.
  ///
  /// \return An expected wrapper
  [[nodiscard]] constexpr auto operator*() const noexcept -> reference {
    constexpr auto u8_code_unit = [] (auto code_point, auto octet_index) -> tl::expected<char, unicode_errc> {
      if (code_point < 0x80u) {
        return static_cast<char>(code_point);
      } else if (code_point < 0x800u) {
        if (octet_index == 0) {
          return static_cast<char>((code_point >> 6u) | 0xc0u);
        } else if (octet_index == 1) {
          return static_cast<char>((code_point & 0x3fu) | 0x80u);
        }
      } else if (code_point < 0x10000u) {
        if (octet_index == 0) {
          return static_cast<char>((code_point >> 12u) | 0xe0u);
        } else if (octet_index == 1) {
          return static_cast<char>(((code_point >> 6u) & 0x3fu) | 0x80u);
        } else if (octet_index == 2) {
          return static_cast<char>((code_point & 0x3fu) | 0x80u);
        }
      } else {
        if (octet_index == 0) {
          return static_cast<char>((code_point >> 18u) | 0xf0u);
        } else if (octet_index == 1) {
          return static_cast<char>(((code_point >> 12u) & 0x3fu) | 0x80u);
        } else if (octet_index == 2) {
          return static_cast<char>(((code_point >> 6u) & 0x3fu) | 0x80u);
        } else if (octet_index == 3) {
          return static_cast<char>((code_point & 0x3fu) | 0x80u);
        }
      }
      return tl::make_unexpected(unicode_errc::invalid_code_point);
    };

    return u8_code_unit(u32_value(*it_).value(), octet_index_);
  }

  /// Equality operator
  /// \param other The other iterator
  /// \return \c true if the iterators are the same, \c false otherwise
  constexpr auto operator==([[maybe_unused]] sentinel sentinel) const noexcept {
    return (it_ == last_);
  }

  /// Inequality operator
  /// \param other The other iterator
  /// \return \c `!(*this == other)`
  constexpr auto operator!=(sentinel sentinel) const noexcept {
    return !(*this == sentinel);
  }

 private:

  constexpr void increment() {
    constexpr auto octet_count = [] (char32_t code_point) {
      if (code_point < 0x80u) {
        return 1;
      } else if (code_point < 0x800u) {
        return 2;
      } else if (code_point < 0x10000u) {
        return 3;
      } else {
        return 4;
      };
    };

    ++octet_index_;
    if (octet_index_ == octet_count(u32_value(*it_).value())) {
      octet_index_ = 0;
      ++it_;
    }
  }

  CodePointIterator it_;
  Sentinel last_;
  int octet_index_ = 0;

};


/// A range that transforms code point values to a UTF-8 sequence
/// \tparam CodePointRange
template<class CodePointRange>
class transform_u8_range {


  using iterator_type = u8_transform_iterator<
      traits::range_iterator_t<CodePointRange>,
      decltype(std::cend(std::declval<CodePointRange>()))>;

 public:

  /// An expected wrapper around a UTF-8 value
  using value_type = tl::expected<char, unicode_errc>;
  /// \c value_type
  using const_reference = value_type;
  /// \c const_reference
  using reference = const_reference;
  /// \c const value_type*
  using const_pointer = const value_type *;
  /// \c const value_type*
  using pointer = const_pointer;
  /// \c transform_u8_iterator
  using const_iterator = iterator_type;
  /// \c const_iterator
  using iterator = const_iterator;
  /// \c std::size_t
  using size_type = std::size_t;

  /// Constructor
  /// \param range A range of code points
  explicit constexpr transform_u8_range(
      const CodePointRange &range)
      : first_(std::cbegin(range), std::cend(range)) {}

  /// Returns an iterator to the first element in the code point sequence
  /// \return \c const_iterator
  [[nodiscard]] auto cbegin() const noexcept {
    return first_;
  }

  /// Returns an iterator to the last element in the code point sequence
  /// \return \c const_iterator
  [[nodiscard]] auto cend() const noexcept {
    return sentinel{};
  }

  /// Returns an iterator to the first element in the code point sequence
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto begin() const noexcept {
    return cbegin();
  }

  /// Returns an iterator to the last element in the code point sequence
  /// \return \c const_iterator
  [[nodiscard]] constexpr auto end() const noexcept {
    return cend();
  }

  /// Tests if the byte range is empty
  /// \return \c true if the range is empty, \c false otherwise
  [[nodiscard]] constexpr auto empty() const noexcept {
    return begin() == end();
  }

 private:

  iterator_type first_;

};

///
struct u8_range_fn {

  ///
  /// \tparam CodePointRange
  /// \param range
  /// \return
  template<class CodePointRange>
  constexpr auto operator()(
      CodePointRange &&range) const {
    return transform_u8_range{std::forward<CodePointRange>(range)};
  }

  ///
  /// \tparam CodePointRange
  /// \param range
  /// \return
  template<typename CodePointRange>
  friend constexpr auto operator|(
      CodePointRange &&range,
      const u8_range_fn &) {
    return transform_u8_range{std::forward<CodePointRange>(range)};
  }
};

namespace transforms {
static constexpr u8_range_fn to_u8;
}  // namespace transforms

///
/// \tparam Output
/// \tparam CodePointRange
/// \param range
/// \return
template <class Output, typename CodePointRange>
auto as(transform_u8_range<CodePointRange> &&range) -> tl::expected<Output, unicode_errc> {
  auto result = Output{};

  for (auto it = std::cbegin(range); it != std::cend(range); ++it) {
    auto unit = *it;
    if (!unit) {
      return tl::make_unexpected(unit.error());
    }
    result.push_back(
        static_cast<typename Output::value_type>(unit.value()));
  }

  return result;
}
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_RANGES_TRANSFORMS_U8_TRANSFORM_HPP
