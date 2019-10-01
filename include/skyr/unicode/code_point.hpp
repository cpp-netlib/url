// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_CODE_POINT_HPP
#define SKYR_UNICODE_CODE_POINT_HPP

#include <tl/expected.hpp>
#include <range/v3/distance.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/constants.hpp>
#include <skyr/unicode/core.hpp>

namespace skyr::unicode {
/// This class defines a range over a code point in raw bytes,
/// according to UTF-8.
/// \tparam OctetIterator An iterator type over the raw bytes
template<typename OctetIterator>
class u8_code_point_t {
 public:

  ///
  using const_iterator = OctetIterator;
  ///
  using iterator = const_iterator;
  ///
  using value_type = char;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using size_type = std::size_t;

  /// \brief Constructor
  /// \param first An iterator at the beginning of the code point
  /// \param last An iterator at the end of the code point
  constexpr u8_code_point_t(
      OctetIterator first,
      OctetIterator last)
      : first(first), last(last) {}

  /// \brief Constructor. The length of the code point sequence is
  ///        inferred from the first code point value.
  /// \param first An iterator at the beginning of the code point
  explicit constexpr u8_code_point_t(OctetIterator first)
      : u8_code_point_t(first, first + sequence_length(*first)) {}

  /// \brief Copy constructor.
  constexpr u8_code_point_t(const u8_code_point_t &) = default;
  /// \brief Move constructor.
  constexpr u8_code_point_t(u8_code_point_t &&) noexcept = default;
  /// \brief Copy assignment operator.
  constexpr u8_code_point_t &operator=(const u8_code_point_t &) = default;
  /// \brief Move assignment operator.
  constexpr u8_code_point_t &operator=(u8_code_point_t &&) noexcept = default;
  /// \brief Destructor.
  ~u8_code_point_t() = default;

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return first;
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return last;
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

  /// \brief Returns the length in bytes of this code point.
  /// \return
  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return sequence_length(*first);
  }

  [[nodiscard]] tl::expected<char32_t, std::error_code> u32_value() const noexcept {
    return find_code_point(first)
    .and_then([] (auto state) -> tl::expected<char32_t, std::error_code> {
      return state.value;
    });
  }

 private:

  OctetIterator first, last;

};

///
/// \tparam OctetRange
/// \param range
/// \return
template<typename OctetRange>
inline tl::expected<u8_code_point_t<typename OctetRange::const_iterator>, std::error_code> u8_code_point(
    const OctetRange &range) {
  auto first = std::begin(range);
  if (ranges::distance(range) > sequence_length(*first)) {
    return tl::make_unexpected(make_error_code(unicode_errc::overflow));
  }
  return u8_code_point_t<typename OctetRange::const_iterator>(
      first,
      first + sequence_length(*first));
}


/// Tests if the code point value is valid.
/// \returns \c true if the value is a valid code point, \c false otherwise
template <typename OctetIterator>
inline bool is_valid(const u8_code_point_t<OctetIterator> &code_point) {
  return static_cast<bool>(find_code_point(std::begin(code_point)));
}

///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
inline tl::expected<u8_code_point_t<typename OctetRange::const_iterator>, std::error_code> valid_u8_code_point(
    const OctetRange &range) {
  using result_type = tl::expected<u8_code_point_t<typename OctetRange::const_iterator>, std::error_code>;

  auto check_code_point = [] (auto &&code_point) -> result_type {
    return find_code_point(std::begin(code_point))
        .and_then([=] (auto) -> result_type {
          return code_point;
        });
  };

  return
      u8_code_point(range)
          .and_then(check_code_point);
}

//namespace details {
/////
///// \tparam OctetIterator
///// \param code_point
///// \return
//template<typename OctetIterator>
//inline char32_t u32(u8_code_point_t<OctetIterator> code_point) {
//  auto state = find_code_point(std::begin(code_point));
//  return state ? state.value().value : U'\x0000';
//}
//}  // namespace details

///
class u16_code_point_t {

 public:

  ///
  /// \param code_point
  explicit constexpr u16_code_point_t(char32_t code_point)
  : code_point_(code_point) {}

  ///
  /// \param code_point
  explicit constexpr u16_code_point_t(char16_t code_point)
  : code_point_(code_point) {}

  constexpr u16_code_point_t(char16_t lead_value, char16_t trail_value)
  : code_point_((lead_value << 10) + trail_value + constants::surrogates::offset) {}

  ///
  constexpr u16_code_point_t(const u16_code_point_t &) = default;
  ///
  constexpr u16_code_point_t(u16_code_point_t &&) noexcept = default;
  ///
  u16_code_point_t &operator=(const u16_code_point_t &) = default;
  ///
  u16_code_point_t &operator=(u16_code_point_t &&) noexcept = default;
  ///
  ~u16_code_point_t() = default;

  ///
  /// \return
  [[nodiscard]] uint16_t lead_value() const {
    return is_surrogate_pair()?
           static_cast<char16_t>((code_point_ >> 10U) + constants::surrogates::lead_offset) :
           static_cast<char16_t>(code_point_);
  }

  ///
  /// \return
  [[nodiscard]] uint16_t trail_value() const {
    return is_surrogate_pair()?
           static_cast<char16_t>((code_point_ & 0x3ffU) + constants::surrogates::trail_min) :
           0;
  }

  ///
  /// \return
  [[nodiscard]] constexpr bool is_surrogate_pair() const noexcept {
    return code_point_ > 0xffffU;
  }

  tl::expected<char32_t, std::error_code> u32_value() const noexcept {
    return code_point_;
  }

 private:

  char32_t code_point_;

};

///
/// \param code_point
/// \return
inline u16_code_point_t u16_code_point(char32_t code_point) {
  return u16_code_point_t(code_point);
}

///
/// \param code_point
/// \return
inline u16_code_point_t u16_code_point(char16_t code_point) {
  return u16_code_point_t(code_point);
}

///
/// \param lead
/// \param value
/// \return
inline u16_code_point_t u16_code_point(char16_t lead, char16_t value) {
  return u16_code_point_t(lead, value);
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline tl::expected<char32_t, std::error_code> u32_value(
    u8_code_point_t<OctetIterator> code_point) noexcept {
  return code_point.u32_value();
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline tl::expected<char32_t, std::error_code> u32_value(
    tl::expected<u8_code_point_t<OctetIterator>, std::error_code> code_point) noexcept {
  return code_point
  .and_then([] (auto code_point) -> tl::expected<char32_t , std::error_code> {
    return code_point.u32_value();
  });
}

///
/// \param code_point
/// \return
inline tl::expected<char32_t, std::error_code> u32_value(
    u16_code_point_t code_point) noexcept {
  return code_point.u32_value();
}

///
/// \param code_point
/// \return
inline tl::expected<char32_t, std::error_code> u32_value(
    tl::expected<u16_code_point_t, std::error_code> code_point) noexcept {
  return code_point
  .and_then([] (auto code_point) -> tl::expected<char32_t, std::error_code> {
    return code_point.u32_value();
  });
}

///
/// \param code_point
/// \return
inline tl::expected<char32_t, std::error_code> u32_value(
    char32_t code_point) noexcept {
  return code_point;
}

///
/// \param code_point
/// \return
inline tl::expected<char32_t, std::error_code> u32_value(
    tl::expected<char32_t, std::error_code> code_point) noexcept {
  return code_point;
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline tl::expected<u16_code_point_t, std::error_code> u16_value(
    u8_code_point_t<OctetIterator> code_point) {
  return u16_code_point(u32_value(code_point));
}

///
/// \tparam OctetIterator
/// \param code_point
/// \return
template <typename OctetIterator>
inline tl::expected<u16_code_point_t, std::error_code> u16_value(
    tl::expected<u8_code_point_t<OctetIterator>, std::error_code> code_point) {
  return u32_value(code_point)
  .and_then([] (auto code_point) -> tl::expected<u16_code_point_t, std::error_code> {
    return u16_code_point(code_point);
  });
}
}  // namespace skyr::unicode


#endif //SKYR_UNICODE_CODE_POINT_HPP
