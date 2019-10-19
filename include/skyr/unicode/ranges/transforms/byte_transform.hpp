// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_BYTE_RANGE_HPP
#define SKYR_UNICODE_BYTE_RANGE_HPP

#include <iterator>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/constants.hpp>
#include <skyr/unicode/ranges/traits.hpp>
#include <skyr/unicode/ranges/views/u16_view.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
/// An iterator that converts a code point to bytes when dereferenced
/// \tparam CodePointIterator
template<class CodePointIterator>
class transform_byte_iterator {

 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = tl::expected<char, std::error_code>;
  ///
  using reference = value_type;
  /// \c std::ptrdiff_t
  using difference_type = std::ptrdiff_t;

  /// Constructor
  transform_byte_iterator() = default;
  /// Constructs an iterator from an iterator that iterates over
  /// code points
  /// \param it
  /// \param last
  transform_byte_iterator(
      CodePointIterator it,
      CodePointIterator last)
      : it_(it), last_(last) {}

  /// Pre-increment operator
  /// \return A reference to this iterator
  transform_byte_iterator &operator++() noexcept {
    increment();
    return *this;
  }

  /// Post-increment operator
  /// \return A copy of the previous iterator
  transform_byte_iterator operator++(int) noexcept {
    auto result = *this;
    increment();
    return result;
  }

  /// Dereference operator
  /// \return An expected value
  [[nodiscard]] reference operator*() const noexcept {
    auto code_point = u32_value(*it_).value();

    if (!is_valid_code_point(code_point)) {
      return tl::make_unexpected(make_error_code(unicode_errc::invalid_code_point));
    }

    if (code_point < 0x80u) {
      return static_cast<char>(code_point);
    } else if (code_point < 0x800u) {
      if (octet_index_ == 0) {
        return static_cast<char>((code_point >> 6u) | 0xc0u);
      } else if (octet_index_ == 1) {
        return static_cast<char>((code_point & 0x3fu) | 0x80u);
      }
    } else if (code_point < 0x10000u) {
      if (octet_index_ == 0) {
        return static_cast<char>((code_point >> 12u) | 0xe0u);
      } else if (octet_index_ == 1) {
        return static_cast<char>(((code_point >> 6u) & 0x3fu) | 0x80u);
      } else if (octet_index_ == 2) {
        return static_cast<char>((code_point & 0x3fu) | 0x80u);
      }
    } else {
      if (octet_index_ == 0) {
        return static_cast<char>((code_point >> 18u) | 0xf0u);
      } else if (octet_index_ == 1) {
        return static_cast<char>(((code_point >> 12u) & 0x3fu) | 0x80u);
      } else if (octet_index_ == 2) {
        return static_cast<char>(((code_point >> 6u) & 0x3fu) | 0x80u);
      } else if (octet_index_ == 3) {
        return static_cast<char>((code_point & 0x3fu) | 0x80u);
      }
    }
    return tl::make_unexpected(make_error_code(unicode_errc::invalid_code_point));
  }

  /// Equality operator
  /// \param other The other iterator
  /// \return \c true if the iterators are the same, \c false otherwise
  constexpr bool operator==(const transform_byte_iterator &other) const noexcept {
    return (it_ == other.it_) && (octet_index_ == other.octet_index_);
  }

  /// Inequality operator
  /// \param other The other iterator
  /// \return \c true if the iterators are not the same, \c false otherwise
  constexpr bool operator!=(const transform_byte_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  static constexpr auto octet_count(char32_t code_point) {
    if (code_point < 0x80u) {
      return 1;
    } else if (code_point < 0x800u) {
      return 2;
    } else if (code_point < 0x10000u) {
      return 3;
    } else {
      return 4;
    };
  }

  void increment() {
    if (**this) {
      ++octet_index_;
      if (octet_index_ == octet_count(u32_value(*it_).value())) {
        octet_index_ = 0;
        ++it_;
      }
    } else {
      it_ = last_;
    }
  }

  CodePointIterator it_, last_;
  int octet_index_ = 0;

};


/// A range that transforms code point values to bytes.
/// \tparam CodePointRange
template<class CodePointRange>
class transform_byte_range {

  using iterator_type = transform_byte_iterator<typename traits::iterator<CodePointRange>::type>;

 public:

  /// An expected byte value
  using value_type = tl::expected<char, std::error_code>;
  /// \c value_type
  using const_reference = value_type;
  /// \c const_reference
  using reference = const_reference;
  /// \c value_type*
  using pointer = typename std::add_pointer<value_type>::type;
  /// \c transform_byte_iterator
  using const_iterator = iterator_type;
  /// \c const_iterator
  using iterator = const_iterator;
  /// \c std::size_t
  using size_type = std::size_t;

  /// Default constructor
  /// \post empty()
  transform_byte_range() = default;

  ///
  /// \param range
  explicit transform_byte_range(
      const CodePointRange &range) noexcept
      : first(iterator_type{std::begin(range), std::end(range)}),
        last(iterator_type{std::end(range), std::end(range)}) {}

  /// Returns an iterator to the beginning
  /// \return \c const_iterator
  [[nodiscard]] const_iterator begin() const noexcept {
    return first ? first.value() : iterator_type();
  }

  /// Returns an iterator to the end
  /// \return \c const_iterator
  [[nodiscard]] const_iterator end() const noexcept {
    return last ? last.value() : iterator_type();
  }

  /// Returns an iterator to the beginning
  /// \return \c const_iterator
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return begin();
  }

  /// Returns an iterator to the end
  /// \return \c const_iterator
  [[nodiscard]] const_iterator cend() const noexcept {
    return end();
  }

  /// Tests if the byte range is empty
  /// \return \c true if the range is empty, \c false otherwise
  [[nodiscard]] bool empty() const noexcept {
    return begin() == end();
  }

 private:

  std::optional<iterator_type> first, last;

};

///
struct byte_range_fn {

  ///
  /// \tparam CodePointRange
  /// \param range
  /// \return
  template<class CodePointRange>
  constexpr auto operator()(
      CodePointRange &&range) const noexcept {
    return transform_byte_range{std::forward<CodePointRange>(range)};
  }

  ///
  /// \tparam CodePointRange
  /// \param range
  /// \return
  template<typename CodePointRange>
  friend constexpr auto operator|(
      CodePointRange &&range,
      const byte_range_fn &) noexcept {
    return transform_byte_range{std::forward<CodePointRange>(range)};
  }
};

namespace transform {
static constexpr byte_range_fn to_bytes;
}  // namespace transform

///
/// \tparam Output
/// \tparam CodePointRange
/// \param range
/// \return
template <class Output, typename CodePointRange>
tl::expected<Output, std::error_code> as(
    transform_byte_range<CodePointRange> &&range) {
  auto result = Output{};
  for (auto &&byte : range) {
    if (!byte) {
      return tl::make_unexpected(byte.error());
    }
    result.push_back(byte.value());
  }
  return result;
}
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif //SKYR_UNICODE_BYTE_RANGE_HPP
