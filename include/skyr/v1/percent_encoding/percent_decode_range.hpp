// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_PERCENT_ENCODING_PERCENT_DECODE_RANGE_HPP
#define SKYR_V1_PERCENT_ENCODING_PERCENT_DECODE_RANGE_HPP

#include <iterator>
#include <string_view>
#include <cassert>
#include <tl/expected.hpp>
#include <skyr/v1/percent_encoding/errors.hpp>
#include <skyr/v1/percent_encoding/sentinel.hpp>

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
  using const_pointer = const value_type *;
  ///
  using pointer = const_pointer;
  ///
  using difference_type = std::ptrdiff_t;
  ///
  using size_type = std::size_t;

  ///
  explicit percent_decode_iterator(std::string_view s)
  : remainder_(s) {}

  ///
  /// \return
  auto operator++(int) noexcept -> percent_decode_iterator {
    assert(!remainder_.empty());
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  auto operator++() noexcept -> percent_decode_iterator & {
    assert(!remainder_.empty());
    increment();
    return *this;
  }

  ///
  /// \return
  [[nodiscard]] auto operator * () const noexcept -> const_reference {
    assert(!remainder_.empty());

    if (remainder_[0] == '%') {
      if (remainder_.size() < 3) {
        return tl::make_unexpected(percent_encoding::percent_encode_errc::overflow);
      }
      auto v0 = details::letter_to_hex(remainder_[1]);
      auto v1 = details::letter_to_hex(remainder_[2]);

      if (!v0 || !v1) {
        return tl::make_unexpected(percent_encoding::percent_encode_errc::non_hex_input);
      }

      return static_cast<char>((0x10u * v0.value()) + v1.value());
    } else {
      return remainder_[0];
    }
  }

  ///
  /// \param sentinel
  /// \return
  auto operator==([[maybe_unused]] sentinel sentinel) const noexcept -> bool {
    return remainder_.empty();
  }

  ///
  /// \param sentinel
  /// \return
  auto operator!=(sentinel sentinel) const noexcept -> bool {
    return !(*this == sentinel);
  }

 private:

  void increment() {
    auto step = (remainder_[0] == '%') ? 3u : 1u;
    remainder_.remove_prefix(step);
  }

  std::string_view remainder_;

};

///
class percent_decode_range {

  using iterator_type = percent_decode_iterator;

 public:

  ///
  using const_iterator = iterator_type;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  /// \param range
  explicit percent_decode_range(std::string_view s)
      : it_(s) {}

  ///
  /// \return
  [[nodiscard]] auto cbegin() const noexcept {
    return it_;
  }

  ///
  /// \return
  [[nodiscard]] auto cend() const noexcept {
    return sentinel{};
  }

  ///
  /// \return
  [[nodiscard]] auto begin() const noexcept {
    return cbegin();
  }

  ///
  /// \return
  [[nodiscard]] auto end() const noexcept {
    return cend();
  }

 private:

  iterator_type it_;

};
}  // namespace percent_encoding
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_PERCENT_ENCODING_PERCENT_DECODE_RANGE_HPP
