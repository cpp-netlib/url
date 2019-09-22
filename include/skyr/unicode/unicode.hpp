// Copyright 2018-19 Glyn Matthews.
// Copyright 2006-2016 Nemanja Trifunovic
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_HPP
#define SKYR_UNICODE_HPP

#include <string>
#include <string_view>
#include <system_error>
#include <tl/expected.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/view_facade.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>

namespace skyr::unicode {
/// Copies characters from a UTF-16 encoded string to a UTF-8
/// encoded string.
///
/// \tparam U16BitIterator
/// \tparam OctetIterator
/// \param first The first iterator in the UTF-16 encoded sequence
/// \param last The last iterator in the UTF-16 encoded sequence
/// \param u8_it The output iterator
/// \return The last output iterator or an error if the sequence was invalid
template <typename U16BitIterator, typename OctetIterator>
tl::expected<OctetIterator, std::error_code> copy_u16u8(
    U16BitIterator first,
    U16BitIterator last,
    OctetIterator result) {
  auto it = first;
  while (it != last) {
    auto code_point = static_cast<std::uint32_t>(mask16(*it));
    ++it;

    // Take care of surrogate pairs first
    if (is_lead_surrogate(code_point)) {
      if (it == last) {
        return tl::make_unexpected(
            make_error_code(unicode_errc::invalid_code_point));
      }

      auto trail_surrogate = mask16(*it);
      ++it;
      if (!is_trail_surrogate(trail_surrogate)) {
        return tl::make_unexpected(
            make_error_code(unicode_errc::invalid_code_point));
      }
      code_point = (code_point << 10) + trail_surrogate + constants::surrogates::offset;
    }
    else if (is_trail_surrogate(code_point)) {
      return tl::make_unexpected(
          make_error_code(unicode_errc::invalid_code_point));
    }

    auto result_it = append_bytes(code_point, result);
    if (!result_it) {
      return tl::make_unexpected(std::move(result_it.error()));
    }
  }
  return result;
}

/// Copies characters from a UTF-8 encoded string to a UTF-16
/// encoded string.
///
/// \tparam U16BitIterator
/// \tparam OctetIterator
/// \param first The first iterator in the octet sequence
/// \param last The last iterator in the octet sequence
/// \param u16_first The first iterator in the UTf-16 encoded
///        sequence
/// \return An expected iterator to the last eleent in the new
///         UTF-16 sequence, or an error.
template <typename U16BitIterator, typename OctetIterator>
tl::expected<U16BitIterator, std::error_code> copy_u8u16(
    OctetIterator first,
    OctetIterator last,
    U16BitIterator u16_first) {
  auto it = first;
  auto u16_it = u16_first;
  while (it != last) {
    if (std::distance(it, last) < sequence_length(*it)) {
      return tl::make_unexpected(make_error_code(unicode_errc::overflow));
    }

    auto state = unicode::next(it);
    if (!state) {
      return tl::make_unexpected(std::move(state.error()));
    }

    it = state.value().it;
    if (state.value().value > 0xffff) {  // make a surrogate pair
      *u16_it++ =
          static_cast<char16_t>((state.value().value >> 10) +
              constants::surrogates::lead_offset);
      *u16_it++ =
          static_cast<char16_t>((state.value().value & 0x3ff) +
              constants::surrogates::trail_min);
    } else {
      *u16_it++ = static_cast<char16_t>(state.value().value);
    }
  }
  return u16_it;
}

/// Copies characters from a UTF-32 encoded string to a UTF-8
/// encoded string.
///
/// \tparam OctetIterator
/// \tparam U32BitIterator
/// \param first The first iterator in the UTF-32 encoded sequence
/// \param last The last iterator in the UTF-32 encoded sequence
/// \param u8_it The output iterator
/// \return The last output iterator or an error if the sequence was invalid
template <typename OctetIterator, typename U32BitIterator>
tl::expected<OctetIterator, std::error_code> copy_u32u8(
    U32BitIterator first,
    U32BitIterator last,
    OctetIterator u8_it) {
  auto it = first;
  while (it != last) {
    auto result_it = append_bytes(*it, u8_it);
    if (!result_it) {
      return tl::make_unexpected(std::move(result_it.error()));
    }
    u8_it = result_it.value();
    ++it;
  }
  return u8_it;
}

/// Copies characters from a UTF-8 encoded string to a UTF-32
/// encoded string.
///
/// \tparam OctetIterator
/// \tparam U32BitIterator
/// \param first The first iterator in the octet sequence
/// \param last The last iterator in the octet sequence
/// \param u32_first The first iterator in the UTf-32 encoded
///        sequence
/// \return An expected iterator to the last eleent in the new
///         UTF-32 sequence, or an error.
template <typename OctetIterator, typename U32BitIterator>
tl::expected<U32BitIterator, std::error_code> copy_u8u32(
    OctetIterator first,
    OctetIterator last,
    U32BitIterator u32_first) {
  auto it = first;
  auto u32_it = u32_first;
  while (it != last) {
    if (std::distance(it, last) < sequence_length(*it)) {
      return tl::make_unexpected(make_error_code(unicode_errc::overflow));
    }

    auto state = unicode::next(it);
    if (!state) {
      return tl::make_unexpected(std::move(state.error()));
    }
    it = state.value().it;
    (*u32_it)++ = state.value().value;
  }
  return u32_it;
}

/// Converts a `std::string` (assuming UTF-8) string to UTF-16
/// \param input A UTF-8 string
/// \returns A UTF-16 `std::wstring` or an error on failure
tl::expected<std::wstring, std::error_code> wstring_from_bytes(
    std::string_view input);

/// Converts a `std::u16string` string to UTF-8
///
/// \param input A UTF-16 string
/// \returns A UTF-8 `std::string` or an error on failure
tl::expected<std::string, std::error_code> wstring_to_bytes(
    std::wstring_view input);

/// Converts a `std::string` (assuming UTF-8) string to UTF-16
///
/// \param input A UTF-8 string
/// \returns A UTF-16 `std::u16string` or an error on failure
tl::expected<std::u16string, std::error_code> utf16_from_bytes(
    std::string_view input);

/// Converts a `std::u16string` string to UTF-8
///
/// \param input A UTF-16 string
/// \returns A UTF-8 `std::string` or an error on failure
tl::expected<std::string, std::error_code> utf16_to_bytes(
    std::u16string_view input);

/// Converts a `std::string` (assuming UTF-8) string to UTF-32
///
/// \param input A UTF-8 string
/// \returns A UTF-32 `std::u32string` or an error on failure
tl::expected<std::u32string, std::error_code> utf32_from_bytes(
    std::string_view input);

/// Converts a `std::u32string` string to UTF-8
///
/// \param input A UTF-32 string
/// \returns A UTF-8 `std::string` or an error on failure
tl::expected<std::string, std::error_code> utf32_to_bytes(
    std::u32string_view input);
}  // namespace skyr::unicode

#endif //SKYR_UNICODE_HPP
