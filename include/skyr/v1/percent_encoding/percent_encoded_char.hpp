// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_PERCENT_ENCODING_PERCENT_ENCODED_CHAR_HPP
#define SKYR_V1_PERCENT_ENCODING_PERCENT_ENCODED_CHAR_HPP

#include <string>
#include <locale>
#include <cstddef>

namespace skyr {
inline namespace v1 {
namespace percent_encoding {
namespace details {
///
/// \param value
/// \return
inline constexpr auto hex_to_alnum(std::byte value) noexcept {
  if ((value >= std::byte(0x00)) && (value < std::byte(0x0a))) {
    return static_cast<char>(std::to_integer<unsigned>(value) + '0');
  }

  if ((value >= std::byte(0x0a)) && (value < std::byte(0x10))) {
    return static_cast<char>(std::to_integer<unsigned>(value) - '\x0a' + 'A');
  }

  return static_cast<char>(value);
}

///
/// \param value
/// \return
inline constexpr auto is_c0_control_byte(std::byte value) noexcept {
  return (value <= std::byte(0x1f)) || (value > std::byte(0x7e));
}

///
/// \param value
/// \return
inline constexpr auto is_fragment_byte(std::byte value) {
  return
      is_c0_control_byte(value) ||
      (value == std::byte(0x20)) ||
      (value == std::byte(0x22)) ||
      (value == std::byte(0x3c)) ||
      (value == std::byte(0x3e)) ||
      (value == std::byte(0x60));
}

///
/// \param value
/// \return
inline constexpr auto is_query_byte(std::byte value) {
  return
      is_c0_control_byte(value) ||
      (value == std::byte(0x20)) ||
      (value == std::byte(0x22)) ||
      (value == std::byte(0x23)) ||
      (value == std::byte(0x3c)) ||
      (value == std::byte(0x3e));
}

///
/// \param value
/// \return
inline constexpr auto is_special_query_byte(std::byte value) {
  return
      is_query_byte(value) ||
      (value == std::byte(0x27));
}

///
/// \param value
/// \return
inline constexpr auto is_path_byte(std::byte value) {
  return
      is_query_byte(value) ||
      (value == std::byte(0x3f)) ||
      (value == std::byte(0x60)) ||
      (value == std::byte(0x7b)) ||
      (value == std::byte(0x7d));
}

///
/// \param value
/// \return
inline constexpr auto is_userinfo_byte(std::byte value) {
  return
      is_path_byte(value) ||
      (value == std::byte(0x2f)) ||
      (value == std::byte(0x3a)) ||
      (value == std::byte(0x3b)) ||
      (value == std::byte(0x3d)) ||
      (value == std::byte(0x40)) ||
      (value == std::byte(0x5b)) ||
      (value == std::byte(0x5c)) ||
      (value == std::byte(0x5d)) ||
      (value == std::byte(0x5e)) ||
      (value == std::byte(0x7c));
}

///
/// \param value
/// \return
inline constexpr auto is_component_byte(std::byte value) {
  return
      is_userinfo_byte(value) ||
      (value == std::byte(0x24)) ||
      (value == std::byte(0x25)) ||
      (value == std::byte(0x26)) ||
      (value == std::byte(0x2b)) ||
      (value == std::byte(0x2c));
}
}  // namespace details

///
enum class encode_set {
  ///
  any = 0,
  ///
  c0_control,
  ///
  fragment,
  ///
  query,
  ///
  special_query,
  ///
  path,
  ///
  userinfo,
  ///
  component,
};

///
struct percent_encoded_char {

  using impl_type = std::string;

  static constexpr std::byte mask = std::byte(0x0f);

 public:

  ///
  using const_iterator = impl_type::const_iterator;
  ///
  using iterator = const_iterator;
  ///
  using size_type = impl_type::size_type;
  ///
  using difference_type = impl_type::difference_type;

  ///
  struct no_encode {};

  ///
  percent_encoded_char() = default;

  ///
  /// \param value
  percent_encoded_char(std::byte value, no_encode)
      : impl_{static_cast<char>(value)} {}

  ///
  /// \param value
  explicit percent_encoded_char(std::byte value)
      : impl_{
      '%', details::hex_to_alnum((value >> 4u) & mask), details::hex_to_alnum(value & mask)} {}

  ///
  /// \return
  [[nodiscard]] auto cbegin() const noexcept {
    return impl_.cbegin();
  }

  ///
  /// \return
  [[nodiscard]] auto cend() const noexcept {
    return impl_.cend();
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

  ///
  /// \return
  [[nodiscard]] auto size() const noexcept {
    return impl_.size();
  }

  ///
  /// \return
  [[nodiscard]] auto is_encoded() const noexcept {
    return impl_.size() == 3;
  }

  ///
  /// \return
  [[nodiscard]] auto to_string() const & -> std::string {
    return impl_;
  }

  ///
  /// \return
  [[nodiscard]] auto to_string() && noexcept -> std::string && {
    return std::move(impl_);
  }

 private:

  impl_type impl_;

};

///
/// \tparam Pred
/// \param byte
/// \param pred
/// \return
template <class Pred>
inline auto percent_encode_byte(std::byte byte, Pred pred) -> percent_encoded_char {
  if (pred(byte)) {
    return percent_encoding::percent_encoded_char(byte);
  }
  return percent_encoding::percent_encoded_char(
      byte, percent_encoding::percent_encoded_char::no_encode());
}

///
/// \param value
/// \param encodes
/// \return
inline auto percent_encode_byte(std::byte value, encode_set encodes) -> percent_encoded_char {
  switch (encodes) {
    case encode_set::any:
      return percent_encoding::percent_encoded_char(value);
    case encode_set::c0_control:
      return percent_encode_byte(value, details::is_c0_control_byte);
    case encode_set::component:
      return percent_encode_byte(value, details::is_component_byte);
    case encode_set::userinfo:
      return percent_encode_byte(value, details::is_userinfo_byte);
    case encode_set::path:
      return percent_encode_byte(value, details::is_path_byte);
    case encode_set::special_query:
      return percent_encode_byte(value, details::is_special_query_byte);
    case encode_set::query:
      return percent_encode_byte(value, details::is_query_byte);
    case encode_set::fragment:
      return percent_encode_byte(value, details::is_fragment_byte);
  }
  return percent_encoding::percent_encoded_char(value);
}

/// Tests whether the input string contains percent encoded values
/// \param input An ASCII string
/// \returns `true` if the input string contains percent encoded
///          values, `false` otherwise
inline auto is_percent_encoded(std::string_view input) noexcept {
  return
      (input.size() == 3) &&
      (input[0] == '%') &&
      std::isxdigit(input[1], std::locale::classic()) &&
      std::isxdigit(input[2], std::locale::classic());
}
}  // namespace percent_encoding
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_PERCENT_ENCODING_PERCENT_ENCODED_CHAR_HPP
