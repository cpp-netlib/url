// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_PERCENT_ENCODING_PERCENT_ENCODE_RANGE_HPP
#define SKYR_V1_PERCENT_ENCODING_PERCENT_ENCODE_RANGE_HPP

#include <iterator>
#include <cassert>
#include <optional>
#include <skyr/v1/percent_encoding/errors.hpp>
#include <skyr/v1/percent_encoding/percent_encoded_char.hpp>

namespace skyr {
inline namespace v1 {
namespace percent_encoding {
///
/// \tparam OctetIterator
template <class OctetIterator>
class percent_encode_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = percent_encoded_char;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  percent_encode_iterator() = default;
  ///
  /// \param it
  /// \param last
  percent_encode_iterator(
      OctetIterator it,
      OctetIterator last)
  : it_(it)
  , last_(last) {}
  ///
  percent_encode_iterator(const percent_encode_iterator &) = default;
  ///
  percent_encode_iterator(percent_encode_iterator &&) noexcept = default;
  ///
  percent_encode_iterator &operator=(const percent_encode_iterator &) = default;
  ///
  percent_encode_iterator &operator=(percent_encode_iterator &&) noexcept = default;
  ///
  ~percent_encode_iterator() = default;

  ///
  /// \return
  auto &operator++() noexcept {
    assert(it_);
    increment();
    return *this;
  }

  ///
  /// \return
  auto operator++(int) noexcept {
    assert(it_);
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  [[nodiscard]] auto operator*() const noexcept -> const_reference {
    assert(it_);
    auto byte = *it_.value();
    if (byte == '\x20') {
      return percent_encoded_char('+', percent_encoded_char::no_encode());
    } else if ((byte == '\x2a') || (byte == '\x2d') || (byte == '\x2e') ||
               ((byte >= '\x30') && (byte <= '\x39')) ||
               ((byte >= '\x41') && (byte <= '\x5a')) || (byte == '\x5f') ||
               ((byte >= '\x61') && (byte <= '\x7a'))) {
      return percent_encoded_char(
          byte, percent_encoded_char::no_encode());
    }
    return percent_encoded_char(byte);
  }

  [[nodiscard]] auto operator==(const percent_encode_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  [[nodiscard]] auto operator!=(const percent_encode_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    ++it_.value();
    if (it_ == last_) {
      it_ = std::nullopt;
    }
  }

  std::optional<OctetIterator> it_, last_;

};

///
/// \tparam OctetRange
template <class OctetRange>
class percent_encode_range {

  using octet_iterator_type = typename OctetRange::const_iterator;
  using iterator_type = percent_encode_iterator<octet_iterator_type>;

 public:

  ///
  using const_iterator = iterator_type;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  percent_encode_range() = default;
  ///
  /// \param range
  explicit percent_encode_range(
      const OctetRange &range)
      : impl_(impl(std::begin(range), std::end(range))) {}

  ///
  /// \return
  [[nodiscard]] auto begin() const noexcept {
    return impl_? impl_.value().first : iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] auto end() const noexcept {
    return iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] auto cbegin() const noexcept {
    return begin();
  }

  ///
  /// \return
  [[nodiscard]] auto cend() const noexcept {
    return end();
  }

  ///
  /// \return
  [[nodiscard]] auto empty() const noexcept {
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
        : first(first, last)
        , last(last, last) {}
    iterator_type first, last;
  };

  std::optional<impl> impl_;

};

struct percent_encode_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(
      const OctetRange &range) const {
    return percent_encode_range{range};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(
      const OctetRange &range,
      const percent_encode_fn&) {
    return percent_encode_range{range};
  }
};

namespace view {
static constexpr percent_encode_fn encode;
}  // namespace view

///
/// \tparam Output
/// \tparam OctetRange
/// \param range
/// \return
template <class Output, class OctetRange>
auto as(
    percent_encode_range<OctetRange> &&range) {
  auto result = Output();
  for (auto &&byte : range) {
    std::copy(
        std::begin(byte),
        std::end(byte),
        std::back_inserter(result));
  }
  return result;
}
}  // namespace percent_encoding
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_PERCENT_ENCODING_PERCENT_ENCODE_RANGE_HPP
