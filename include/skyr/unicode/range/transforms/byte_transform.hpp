// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_BYTE_RANGE_HPP
#define SKYR_UNICODE_BYTE_RANGE_HPP

#include <iterator>
#include <tl/expected.hpp>
#include <range/v3/range_traits.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/constants.hpp>
#include <skyr/unicode/range/traits.hpp>
#include <skyr/unicode/range/views/u16_view.hpp>

namespace skyr::unicode {
template<class CodePointIterator>
class transform_byte_iterator {

 public:

  using value_type = tl::expected<char, std::error_code>;
  using reference = value_type;
  using difference_type = std::ptrdiff_t;

  transform_byte_iterator() = default;

  transform_byte_iterator(CodePointIterator first, CodePointIterator last)
      : it_(first), last_(last) {}

  transform_byte_iterator(const transform_byte_iterator &) = default;
  transform_byte_iterator(transform_byte_iterator &&) noexcept = default;
  transform_byte_iterator &operator=(const transform_byte_iterator &) = default;
  transform_byte_iterator &operator=(transform_byte_iterator &&) noexcept = default;
  ~transform_byte_iterator() = default;

  transform_byte_iterator &operator++() {
    increment();
    return *this;
  }

  transform_byte_iterator operator++(int) {
    auto result = *this;
    increment();
    return result;
  }

  reference operator*() {
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

  constexpr bool operator==(const transform_byte_iterator &other) const noexcept {
    return (it_ == other.it_) && (octet_index_ == other.octet_index_);
  }

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


///
template<class CodePointRange>
class transform_byte_range {

  using iterator_type = transform_byte_iterator<typename traits::iterator<CodePointRange>::type>;

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
  transform_byte_range() = default;

  ///
  /// \param range
  explicit transform_byte_range(
      const CodePointRange &range)
      : first(iterator_type{std::begin(range), std::end(range)}),
        last(iterator_type{std::end(range), std::end(range)}) {}

  ///
  /// \return
  const_iterator begin() const {
    return first ? first.value() : iterator_type();
  }

  ///
  /// \return
  const_iterator end() const {
    return last ? last.value() : iterator_type();
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
  /// \tparam CodePointRange
  /// \param range
  /// \return
  template<class CodePointRange>
  constexpr auto operator()(CodePointRange &&range) const {
    return transform_byte_range{std::forward<CodePointRange>(range)};
  }

  ///
  /// \tparam CodePointRange
  /// \param range
  /// \return
  template<typename CodePointRange>
  friend constexpr auto operator|(CodePointRange &&range, const byte_range_fn &) {
    return transform_byte_range{std::forward<CodePointRange>(range)};
  }
};

namespace transform {
static constexpr byte_range_fn to_bytes;
}  // namespace transform

template <class Output, typename CodePointRange>
tl::expected<Output, std::error_code> as(transform_byte_range<CodePointRange> &&range) {
  auto result = Output{};
  for (auto &&byte : range) {
    if (!byte) {
      return tl::make_unexpected(byte.error());
    }
    result.push_back(byte.value());
  }
  return result;
}
}  // namespace skyr::unicode

#endif //SKYR_UNICODE_BYTE_RANGE_HPP
