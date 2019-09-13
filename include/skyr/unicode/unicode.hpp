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
#include <skyr/unicode/constants.hpp>

namespace skyr {
/// \namespace unicode
/// Unicode encoding functions
namespace unicode {
/// Enumerates Unicode errors
enum class unicode_errc {
  /// Overflow
  overflow,
  /// Invalid lead code point
  invalid_lead,
  /// Illegal byte sequence
  illegal_byte_sequence,
  /// Invalid code point
  invalid_code_point,
};
}  // namespace unicode
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::unicode::unicode_errc> : true_type {};
}  // namespace std

namespace skyr::unicode {
/// Creates a `std::error_code` given a `skyr::unicode_errc` value
/// \param error A Unicode error
/// \returns A `std::error_code` object
std::error_code make_error_code(unicode_errc error);

///
/// \param octet
/// \return
constexpr uint8_t mask8(char octet) {
  return 0xffu & static_cast<uint8_t>(octet);
}

///
/// \param octet
/// \return
constexpr char16_t mask16(char16_t octet) {
  return 0xffffu & octet;
}

///
/// \param octet
/// \return
constexpr bool is_trail(char octet) {
  return ((mask8(octet) >> 6u) == 0x2u);
}

///
/// \param code_point
/// \return
constexpr bool is_lead_surrogate(char16_t code_point) {
  return
      (code_point >= constants::surrogates::lead_min) &&
          (code_point <= constants::surrogates::lead_max);
}

///
/// \param code_point
/// \return
constexpr bool is_trail_surrogate(char16_t code_point) {
  return
      (code_point >= constants::surrogates::trail_min) &&
          (code_point <= constants::surrogates::trail_max);
}

///
/// \param code_point
/// \return
constexpr bool is_surrogate(char16_t code_point) {
  return
      (code_point >= constants::surrogates::lead_min) &&
          (code_point <= constants::surrogates::trail_max);
}

/// Tests if the code point is a valid value.
/// \param code_point
/// \return \c true if it has a valid value, \c false otherwise
constexpr bool is_valid_code_point(char32_t code_point) {
  return
      (code_point <= constants::code_points::max) &&
          !is_surrogate(static_cast<char16_t>(code_point));
}

/// Returns the size of the sequnce given the lead octet value.
/// \param lead_value
/// \return 1, 2, 3 or 4
constexpr std::size_t sequence_length(char lead_value) {
  auto lead = mask8(lead_value);
  if (lead < 0x80) {
    return 1;
  } else if ((lead >> 5) == 0x6) {
    return 2;
  } else if ((lead >> 4) == 0xe) {
    return 3;
  } else if ((lead >> 3) == 0x1e) {
    return 4;
  }
  return 0;
}

///
/// \param code_point
/// \param length
/// \return
constexpr bool is_overlong_sequence(
    char32_t code_point,
    std::size_t length) {
  bool result = false;
  result &= (code_point < 0x80) && (length != 1);
  result &= (code_point < 0x800) && (length != 2);
  result &= (code_point < 0x10000) && (length != 3);
  return result;
}

/// A type used to extract a code point value from an octet sequence
/// \tparam OctetIterator
template<typename OctetIterator>
struct sequence_state {
  sequence_state(
      OctetIterator it,
      char32_t value)
      : it(it), value(value) {}

  /// The current iterator
  OctetIterator it;
  /// The (intermediate) value of the code point
  char32_t value;
};

/// Creates an expected state, so that can be chained
/// functional-style.
///
/// \tparam OctetIterator
/// \param it The lead value of the next code point in the octet
///           sequence
/// \return A sequence_state with a value of 0, and the iterator
///         pointing to the lead value
template<class OctetIterator>
tl::expected<sequence_state<OctetIterator>, unicode_errc>
make_state(OctetIterator it) {
  return sequence_state<OctetIterator>(it, 0);
}

/// Updates the value in the sequence state
///
/// \tparam OctetIterator
/// \param state The input state
/// \param value The updated value
/// \return A new state with an updateds value
template<class OctetIterator>
inline sequence_state<OctetIterator> update_value(
    sequence_state<OctetIterator> state,
    char32_t value) {
  return {state.it, value};
}

/// Moves the octet iterator one character ahead
/// \tparam OctetIterator
/// \param state The input state
/// \return The new state with the updated iterator, on an error if
///         the sequence isn't valid
template<typename OctetIterator>
inline tl::expected<sequence_state<OctetIterator>, unicode_errc>
increment(sequence_state<OctetIterator> state) {
  ++state.it;
  if (!is_trail(*state.it)) {
    return tl::make_unexpected(unicode_errc::illegal_byte_sequence);
  }
  return state;
}

/// Checks if the code point value is valid
///
/// \tparam OctetIterator
/// \param state The input state
/// \return The new state
template<typename OctetIterator>
tl::expected<sequence_state<OctetIterator>, unicode_errc>
check_code_point(sequence_state<OctetIterator> state) {
  if (!is_valid_code_point(state.value)) {
    return tl::make_unexpected(unicode_errc::invalid_code_point);
  } else if (is_overlong_sequence(state.value, sequence_length(*state.it))) {
    return tl::make_unexpected(unicode_errc::illegal_byte_sequence);
  }

  return state;
}

namespace details {
///
/// \tparam OctetIterator
/// \param state
/// \return
template<typename OctetIterator>
tl::expected<sequence_state<OctetIterator>, unicode_errc>
inline mask_byte(sequence_state<OctetIterator> state) {
  return update_value(state, mask8(*state.it));
}

/// Converts a two byte code octet sequence to a code point value.
///
/// \tparam OctetIterator
/// \param first
/// \return
template<typename OctetIterator>
tl::expected<sequence_state<OctetIterator>, unicode_errc>
from_two_byte_sequence(OctetIterator first) {
  using result_type = tl::expected<sequence_state<OctetIterator>, unicode_errc>;

  auto set_code_point = [](auto state) -> result_type {
    return update_value(
        state,
        ((state.value << 6) & 0x7ff) + (*state.it & 0x3f));
  };

  return
      make_state(first)
          .and_then(mask_byte<OctetIterator>)
          .and_then(increment<OctetIterator>)
          .and_then(set_code_point);
}

/// Converts a three byte code octet sequence to a code point value.
///
/// \tparam OctetIterator
/// \param first
/// \return
template<typename OctetIterator>
tl::expected<sequence_state<OctetIterator>, unicode_errc>
from_three_byte_sequence(OctetIterator first) {
  using result_type = tl::expected<sequence_state<OctetIterator>, unicode_errc>;

  auto update_code_point_from_second_byte = [](auto state) -> result_type {
    return update_value(
        state,
        ((state.value << 12) & 0xffff) +
            ((mask8(*state.it) << 6) & 0xfff));
  };

  auto set_code_point = [](auto state) -> result_type {
    return update_value(
        state,
        state.value + (*state.it & 0x3f));
  };

  return make_state(first)
      .and_then(mask_byte<OctetIterator>)
      .and_then(increment<OctetIterator>)
      .and_then(update_code_point_from_second_byte)
      .and_then(increment<OctetIterator>)
      .and_then(set_code_point);
}

/// Converts a four byte code octet sequence to a code point value.
///
/// \tparam OctetIterator
/// \param first
/// \return
template<typename OctetIterator>
tl::expected<sequence_state<OctetIterator>, unicode_errc>
from_four_byte_sequence(OctetIterator first) {
  using result_type = tl::expected<sequence_state<OctetIterator>, unicode_errc>;

  auto update_code_point_from_second_byte = [](auto state) -> result_type {
    return update_value(
        state,
        ((state.value << 18) & 0x1fffff) +
            ((mask8(*state.it) << 12) & 0x3ffff));
  };

  auto update_code_point_from_third_byte = [](auto state) -> result_type {
    return update_value(
        state,
        state.value + ((mask8(*state.it) << 6) & 0xfff));
  };

  auto set_code_point = [](auto state) -> result_type {
    return update_value(
        state,
        state.value + (*state.it & 0x3f));
  };

  return
      make_state(first)
          .and_then(mask_byte<OctetIterator>)
          .and_then(increment<OctetIterator>)
          .and_then(update_code_point_from_second_byte)
          .and_then(increment<OctetIterator>)
          .and_then(update_code_point_from_third_byte)
          .and_then(increment<OctetIterator>)
          .and_then(set_code_point);
}
}  // namespace details

/// Finds and computes the next code point value in the octet
/// sequence.
///
/// \tparam OctetIterator
/// \param first
/// \return
template <typename OctetIterator>
tl::expected<sequence_state<OctetIterator>, unicode_errc> find_code_point(
    OctetIterator first) {
  const auto length = sequence_length(*first);
  return
      (length == 1) ? make_state(first).and_then(details::mask_byte<OctetIterator>) :
      (length == 2) ? details::from_two_byte_sequence(first) :
      (length == 3) ? details::from_three_byte_sequence(first) :
      (length == 4) ? details::from_four_byte_sequence(first) :
      tl::make_unexpected(unicode_errc::overflow)
      ;
}

/// Updates the state to next code point
///
/// \tparam OctetIterator
/// \param it An octer iterator
/// \return A sequence state with the computed code point value
template <typename OctetIterator>
tl::expected<sequence_state<OctetIterator>, unicode_errc> next(
    OctetIterator it) {
  using result_type = tl::expected<sequence_state<OctetIterator>, unicode_errc>;

  auto increment = [] (auto state) -> result_type {
    ++state.it;
    return state;
  };

  return
      find_code_point(it)
          .and_then(check_code_point<OctetIterator>)
          .and_then(increment);
}

/// Appends values to a  octet sequence given a code point value
///
/// \tparam OctetIterator
/// \param code_point
/// \param octet_it
/// \return
template <typename OctetIterator>
tl::expected<OctetIterator, unicode_errc> append_bytes(
    char32_t code_point,
    OctetIterator octet_it) {
  if (!is_valid_code_point(code_point)) {
    return tl::make_unexpected(unicode_errc::invalid_code_point);
  }

  auto value = static_cast<std::uint32_t>(code_point);

  if (value < 0x80u) { // one octet
    *(octet_it++) = static_cast<char>(value);
  } else if (value < 0x800u) {  // two octets
    *(octet_it++) = static_cast<char>((value >> 6u) | 0xc0u);
    *(octet_it++) = static_cast<char>((value & 0x3fu) | 0x80u);
  } else if (value < 0x10000u) {  // three octets
    *(octet_it++) = static_cast<char>((value >> 12u) | 0xe0u);
    *(octet_it++) = static_cast<char>(((value >> 6u) & 0x3fu) | 0x80u);
    *(octet_it++) = static_cast<char>((value & 0x3fu) | 0x80u);
  } else {  // four octets
    *(octet_it++) = static_cast<char>((value >> 18u) | 0xf0u);
    *(octet_it++) = static_cast<char>(((value >> 12u) & 0x3fu) | 0x80u);
    *(octet_it++) = static_cast<char>(((value >> 6u) & 0x3fu) | 0x80u);
    *(octet_it++) = static_cast<char>((value & 0x3fu) | 0x80u);
  }
  return octet_it;
}

/// Advances `n` code oints through the octet sequence
/// \tparam OctetIterator
/// \param it An iterator to a lead octet in the octet sequence
/// \param n The number of code points to advance
/// \param last The last iterator in the octet sequence
/// \return The updated iterator or an error if the sequence is
///         invalid
template <typename OctetIterator>
tl::expected<void, unicode_errc> advance(
    OctetIterator& it,
    std::size_t n,
    OctetIterator last) {
  while (n != 0) {
    if (std::distance(it, last) < sequence_length(*it)) {
      return tl::make_unexpected(unicode_errc::overflow);
    }

    auto state = unicode::next(it);
    if (!state) {
      return tl::make_unexpected(std::move(state.error()));
    }
    it = state.value().it;
    --n;
  }

  return {};
}

/// Counts the number of code points in the octet sequence.
///
/// \tparam OctetIterator
/// \param first The first element in the octet sequence
/// \param last The last element in the sequence
/// \return The number of code points or an error if it's not a
///         valid sequence.
template <typename OctetIterator>
tl::expected<std::size_t, unicode_errc> count(
    OctetIterator first,
    OctetIterator last) {
  std::size_t count = 0;
  auto it = first;

  while (it != last) {
    if (std::distance(it, last) < sequence_length(*it)) {
      return tl::make_unexpected(unicode_errc::overflow);
    }

    auto state = unicode::next(it);
    if (!state) {
      return tl::make_unexpected(std::move(state.error()));
    }
    it = state.value().it;
    ++count;
  }
  return count;
}

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
tl::expected<OctetIterator, unicode_errc> copy_u16u8(
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
        return tl::make_unexpected(unicode_errc::invalid_code_point);
      }

      auto trail_surrogate = mask16(*it);
      ++it;
      if (!is_trail_surrogate(trail_surrogate)) {
        return tl::make_unexpected(unicode_errc::invalid_code_point);
      }
      code_point = (code_point << 10) + trail_surrogate + constants::surrogates::offset;
    }
    else if (is_trail_surrogate(code_point)) {
      return tl::make_unexpected(unicode_errc::invalid_code_point);
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
tl::expected<U16BitIterator, unicode_errc> copy_u8u16(
    OctetIterator first,
    OctetIterator last,
    U16BitIterator u16_first) {
  auto it = first;
  auto u16_it = u16_first;
  while (it != last) {
    if (std::distance(it, last) < sequence_length(*it)) {
      return tl::make_unexpected(unicode_errc::overflow);
    }

    auto state = next(it);
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
tl::expected<OctetIterator, unicode_errc> copy_u32u8(
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
tl::expected<U32BitIterator, unicode_errc> copy_u8u32(
    OctetIterator first,
    OctetIterator last,
    U32BitIterator u32_first) {
  auto it = first;
  auto u32_it = u32_first;
  while (it != last) {
    if (std::distance(it, last) < sequence_length(*it)) {
      return tl::make_unexpected(unicode_errc::overflow);
    }

    auto state = next(it);
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
