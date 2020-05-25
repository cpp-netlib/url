// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_PERCENT_ENCODING_PERCENT_DECODE_RANGE_HPP
#define SKYR_V1_PERCENT_ENCODING_PERCENT_DECODE_RANGE_HPP

#include <iterator>
#include <cassert>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/v1/percent_encoding/errors.hpp>
#include <skyr/v1/percent_encoding/percent_encoded_char.hpp>

namespace skyr {
inline namespace v1 {
namespace percent_encoding {
namespace details {
inline auto letter_to_hex(char byte) noexcept -> tl::expected<char, percent_encode_errc> {
  if ((byte >= '0') && (byte <= '9')) {
    return static_cast<char>(byte - '0');
  }

  if ((byte >= 'a') && (byte <= 'f')) {
    return static_cast<char>(byte + '\x0a' - 'a');
  }

  if ((byte >= 'A') && (byte <= 'F')) {
    return static_cast<char>(byte + '\x0a' - 'A');
  }

  return tl::make_unexpected(percent_encoding::percent_encode_errc::non_hex_input);
}
}  // namespace details

///
/// \tparam OctetIterator
template <class OctetIterator>
class percent_decode_iterator {

 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = tl::expected<char, percent_encode_errc>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_pointer = const typename std::add_pointer<value_type>::type;
  ///
  using pointer = const_pointer;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  percent_decode_iterator() = default;
  ///
  percent_decode_iterator(OctetIterator it, OctetIterator last)
  : it_(it)
  , last_(last) {}
  ///
  percent_decode_iterator(const percent_decode_iterator&) = default;
  ///
  percent_decode_iterator(percent_decode_iterator&&) noexcept = default;
  ///
  percent_decode_iterator &operator=(const percent_decode_iterator&) = default;
  ///
  percent_decode_iterator &operator=(percent_decode_iterator&&) noexcept = default;
  ///
  ~percent_decode_iterator() = default;

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
  auto &operator++() noexcept {
    assert(it_);
    increment();
    return *this;
  }

  ///
  /// \return
  [[nodiscard]] auto operator * () const noexcept -> const_reference {
    assert(it_);
    if (*it_.value() == '%') {
      if (std::distance(it_.value(), last_.value()) < 3) {
        return tl::make_unexpected(percent_encoding::percent_encode_errc::overflow);
      }
      auto it = it_.value();
      auto v0 = details::letter_to_hex(*++it);
      auto v1 = details::letter_to_hex(*++it);

      if (!v0 || !v1) {
        return tl::make_unexpected(percent_encoding::percent_encode_errc::non_hex_input);
      }

      return static_cast<char>((0x10u * v0.value()) + v1.value());
    } else {
      return *it_.value();
    }
  }

  ///
  /// \param other
  /// \return
  auto operator==(const percent_decode_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  auto operator!=(const percent_decode_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    assert(**this);
    std::advance(it_.value(), (((*it_.value() == '%')? 3 : 1)));
    if (it_ == last_) {
      it_ = std::nullopt;
    }
  }

  std::optional<OctetIterator> it_, last_;

};

///
/// \tparam OctetRange
template <class OctetRange>
class percent_decode_range {

  using octet_iterator_type = typename OctetRange::const_iterator;
  using iterator_type = percent_decode_iterator<octet_iterator_type>;

 public:

  ///
  using const_iterator = iterator_type;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  percent_decode_range() = default;
  ///
  /// \param range
  percent_decode_range(const OctetRange &range)
      : impl_(impl(std::begin(range), std::end(range))) {}

  ///
  /// \return
  [[nodiscard]] auto begin() const noexcept {
    return impl_?
           ((impl_.value().first != impl_.value().last)? impl_.value().first : iterator_type()) : iterator_type();
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

struct percent_decode_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(
      const OctetRange &range) const {
    return percent_decode_range{range};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(
      const OctetRange &range,
      const percent_decode_fn&) {
    return percent_decode_range{range};
  }

};

namespace view {
static constexpr percent_decode_fn decode;
}  // namespace view

///
/// \tparam Output
/// \tparam OctetRange
/// \param range
/// \return
template <class Output, class OctetRange>
auto as(
    percent_decode_range<OctetRange> &&range) -> tl::expected<Output, percent_encode_errc> {
  auto result = Output();
  for (auto &&byte : range) {
    if (!byte) {
      return tl::make_unexpected(byte.error());
    }
    result.push_back(byte.value());
  }
  return result;
}
}  // namespace percent_encoding
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_PERCENT_ENCODING_PERCENT_DECODE_RANGE_HPP
