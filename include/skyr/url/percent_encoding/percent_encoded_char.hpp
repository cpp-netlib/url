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
inline constexpr char hex_to_letter(char byte) noexcept {
  if ((byte >= 0x00) && (byte < 0x0a)) {
    return static_cast<char>(byte + '0');
  }

  if ((byte >= 0x0a) && (byte < 0x10)) {
    return static_cast<char>(byte - 0x0a + 'A');
  }

  return byte;
}

inline constexpr bool is_c0_control_byte(char byte) noexcept {
  return (byte <= 0x1f) || (byte > 0x7e);
}

inline bool is_fragment_byte(char byte) {
  constexpr std::array<char, 5> set = {0x20, 0x22, 0x3c, 0x3e, 0x60};
  auto it = std::find(begin(set), end(set), byte);
  return is_c0_control_byte(byte) || (it != set.end());
}

inline bool is_query_byte(char byte) {
  return is_fragment_byte(byte) || (byte == 0x27);
}

inline bool is_path_byte(char byte) {
  constexpr std::array<char, 4> set = {0x23, 0x3f, 0x7b, 0x7d};
  auto it = std::find(begin(set), end(set), byte);
  return is_fragment_byte(byte) || (it != set.end());
}

inline bool is_userinfo_byte(char byte) {
  constexpr std::array<char, 10> set = {
      0x2f, 0x3a, 0x3b, 0x3d, 0x40, 0x5b, 0x5c, 0x5d, 0x5e, 0x7c};
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
  percent_encoded_char(percent_encoded_char &&) = default;
  ///
  percent_encoded_char &operator=(const percent_encoded_char &) = default;
  ///
  percent_encoded_char &operator=(percent_encoded_char &&) = default;
  ///
  ~percent_encoded_char() = default;

  ///
  /// \return
  [[nodiscard]] const_iterator begin() const noexcept {
    return impl_.begin();
  }

  ///
  /// \return
  [[nodiscard]] const_iterator end() const noexcept {
    return impl_.end();
  }

  ///
  /// \return
  [[nodiscard]] size_type size() const noexcept {
    return impl_.size();
  }

  ///
  /// \return
  [[nodiscard]] bool is_encoded() const noexcept {
    return impl_.size() == 3;
  }

  ///
  /// \return
  [[nodiscard]] std::string to_string() const & {
    return impl_;
  }

  ///
  /// \return
  [[nodiscard]] std::string &&to_string() && noexcept {
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
inline percent_encoded_char percent_encode_byte(char byte, Pred pred) {
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
inline percent_encoded_char percent_encode_byte(char byte, encode_set excludes) {
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
}

/// Tests whether the input string contains percent encoded values
/// \param input An ASCII string
/// \returns `true` if the input string contains percent encoded
///          values, `false` otherwise
inline bool is_percent_encoded(std::string_view input) noexcept {
    auto first = begin(input), last = end(input);
    auto it = first;

    if (it == last) {
      return false;
    }

    if (*it == '%') {
      ++it;
      if (it != last) {
        if (std::isxdigit(*it, std::locale::classic())) {
          ++it;
          if (it != last) {
            if (std::isxdigit(*it, std::locale::classic())) {
              return true;
            }
          }
        }
      }
    }

    return false;
}
}  // namespace percent_encoding
}  // namespace v1
}  // namespace skyr

#endif //SKYR_PERCENT_ENCODED_CHAR_HPP
