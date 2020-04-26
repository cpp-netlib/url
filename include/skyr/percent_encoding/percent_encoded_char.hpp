// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_PERCENT_ENCODED_CHAR_HPP
#define SKYR_PERCENT_ENCODED_CHAR_HPP

#include <string>
#include <array>
#include <locale>

namespace skyr {
inline namespace v1 {
namespace percent_encoding {
namespace details {
inline constexpr auto hex_to_letter(char byte) noexcept {
  if ((byte >= '\x00') && (byte < '\x0a')) {
    return static_cast<char>(byte + '0');
  }

  if ((byte >= '\x0a') && (byte < '\x10')) {
    return static_cast<char>(byte - '\x0a' + 'A');
  }

  return byte;
}

inline constexpr auto is_c0_control_byte(char byte) noexcept {
  return (byte <= '\x1f') || (byte > '\x7e');
}

inline auto is_fragment_byte(char byte) {
  constexpr static std::array<char, 5> set = {'\x20', '\x22', '\x3c', '\x3e', '\x60'};
  auto it = std::find(begin(set), end(set), byte);
  return is_c0_control_byte(byte) || (it != set.end());
}

inline auto is_query_byte(char byte) {
  return is_fragment_byte(byte) || (byte == '\x27');
}

inline auto is_path_byte(char byte) {
  constexpr static std::array<char, 4> set = {'\x23', '\x3f', '\x7b', '\x7d'};
  auto it = std::find(begin(set), end(set), byte);
  return is_fragment_byte(byte) || (it != set.end());
}

inline auto is_userinfo_byte(char byte) {
  constexpr static std::array<char, 10> set = {
      '\x2f', '\x3a', '\x3b', '\x3d', '\x40', '\x5b', '\x5c', '\x5d', '\x5e', '\x7c'};
  auto it = std::find(begin(set), end(set), byte);
  return is_path_byte(byte) || (it != set.end());
}
}  // namespace details

///
enum class encode_set {
  ///
  none = 0,
  ///
  c0_control,
  ///
  fragment,
  ///
  query,
  ///
  path,
  ///
  userinfo,
};

///
struct percent_encoded_char {

  using impl_type = std::string;

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
  /// \param byte
  percent_encoded_char(char byte, no_encode)
      : impl_{byte} {}
  ///
  /// \param byte
  explicit percent_encoded_char(char byte)
      : impl_{
      '%',
      details::hex_to_letter(static_cast<char>((static_cast<unsigned>(byte) >> 4u) & 0x0fu)),
      details::hex_to_letter(static_cast<char>(static_cast<unsigned >(byte) & 0x0fu))} {}
  ///
  percent_encoded_char(const percent_encoded_char &) = default;
  ///
  percent_encoded_char(percent_encoded_char &&) noexcept = default;
  ///
  percent_encoded_char &operator=(const percent_encoded_char &) = default;
  ///
  percent_encoded_char &operator=(percent_encoded_char &&) noexcept = default;
  ///
  ~percent_encoded_char() = default;

  ///
  /// \return
  [[nodiscard]] auto begin() const noexcept {
    return impl_.begin();
  }

  ///
  /// \return
  [[nodiscard]] auto end() const noexcept {
    return impl_.end();
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
inline auto percent_encode_byte(char byte, Pred pred) -> percent_encoded_char {
  if (pred(byte)) {
    return percent_encoding::percent_encoded_char(byte);
  }
  return percent_encoding::percent_encoded_char(
      byte, percent_encoding::percent_encoded_char::no_encode());
}

///
/// \param byte
/// \param excludes
/// \return
inline auto percent_encode_byte(char byte, encode_set excludes) -> percent_encoded_char {
  switch (excludes) {
    case encode_set::none:
      return percent_encoding::percent_encoded_char(byte);
    case encode_set::c0_control:
      return percent_encode_byte(byte, details::is_c0_control_byte);
    case encode_set::userinfo:
      return percent_encode_byte(byte, details::is_userinfo_byte);
    case encode_set::path:
      return percent_encode_byte(byte, details::is_path_byte);
    case encode_set::query:
      return percent_encode_byte(byte, details::is_query_byte);
    case encode_set::fragment:
      return percent_encode_byte(byte, details::is_fragment_byte);
  }
  return percent_encoding::percent_encoded_char(byte);
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

#endif //SKYR_PERCENT_ENCODED_CHAR_HPP
