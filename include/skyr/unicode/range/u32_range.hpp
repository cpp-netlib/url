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
#include <range/v3/view.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>
#include <skyr/unicode/range/u8_range.hpp>

namespace skyr::unicode {
///
/// \tparam OctetIterator
template <typename OctetIterator>
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
  constexpr u32_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr u32_range_iterator(u8_range_iterator<OctetIterator> it)
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
      return details::u32(code_point);
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

  u8_range_iterator<OctetIterator> it_;

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

  view_u8_range<OctetRange> range_;

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

namespace u32 {
template<class U32Iterator>
class byte_iterator {

 public:

  using value_type = tl::expected<char, std::error_code>;
  using reference = value_type;
  using difference_type = std::ptrdiff_t;

  byte_iterator() = default;

  byte_iterator(U32Iterator first, U32Iterator last)
      : it_(first), last_(last) {}

  byte_iterator(const byte_iterator &) = default;
  byte_iterator(byte_iterator &&) noexcept = default;
  byte_iterator &operator=(const byte_iterator &) = default;
  byte_iterator &operator=(byte_iterator &&) noexcept = default;
  ~byte_iterator() = default;

  byte_iterator &operator++() {
    increment();
    return *this;
  }

  byte_iterator operator++(int) {
    auto result = *this;
    increment();
    return result;
  }

  reference operator*() {
    auto code_point = *it_;

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

  constexpr bool operator == (const byte_iterator &other) const noexcept {
    return (it_ == other.it_) && (octet_index_ == other.octet_index_);
  }

  constexpr bool operator != (const byte_iterator &other) const noexcept {
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
      if (octet_index_ == octet_count(*it_)) {
        octet_index_ = 0;
        ++it_;
      }
    }
    else {
      it_ = last_;
    }
  }

  U32Iterator it_, last_;
  int octet_index_ = 0;

};

///
template <class U32Range>
class view_byte_range {

  using iterator_type = byte_iterator<typename U32Range::const_iterator>;

 public:

  ///
  using value_type = tl::expected<char, std::error_code>;
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
  view_byte_range() = default;

  ///
  /// \param range
  explicit view_byte_range(
      const U32Range &range)
      : first(iterator_type{std::begin(range), std::end(range)})
      , last(iterator_type{std::end(range), std::end(range)}) {}

  ///
  /// \return
  const_iterator begin() const {
    return first? first.value() : iterator_type();
  }

  ///
  /// \return
  const_iterator end() const {
    return last? last.value() : iterator_type();
  }

  ///
  /// \return
  bool empty() const noexcept {
    return begin() == end();
  }

 private:

  std::optional<iterator_type> first, last;

};

///
struct byte_range_fn {
  ///
  /// \tparam U32Range
  /// \param range
  /// \return
  template <typename U32Range>
  constexpr auto operator()(U32Range &&range) const {
    return view_byte_range{std::forward<U32Range>(range)};
  }

  ///
  /// \tparam U32Range
  /// \param range
  /// \return
  template <typename U32Range>
  friend constexpr auto operator|(U32Range &&range, const byte_range_fn&) {
    return view_byte_range{std::forward<U32Range>(range)};
  }

};
}  // namespace u32

namespace view {
///
static constexpr u32_range_fn u32;
static constexpr u32::byte_range_fn bytes;
}  // namespace view

template <class Output, class InputRange>
tl::expected<Output, std::error_code> as(InputRange &&range) {
  auto result = Output{};
//  result.reserve(ranges::size(range));
  for (auto &&code_point : range) {
    if (!code_point) {
      return tl::make_unexpected(code_point.error());
    }
    result.push_back(code_point.value());
  }
  return result;
}
}  // namespace skyr::unicode

#endif //SKYR_UNICODE_U32_RANGE_HPP
