// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_UNICODE_CORE_HPP
#define SKYR_V2_UNICODE_CORE_HPP

#include <string>
#include <type_traits>
#include <tl/expected.hpp>
#include <skyr/v2/unicode/constants.hpp>
#include <skyr/v2/unicode/errors.hpp>

namespace skyr::inline v2::unicode {
///
/// \param octet
/// \return
template <class uintT>
constexpr inline auto mask8(uintT value) {
  static_assert(std::is_unsigned_v<uintT>, "unsigned integral types only");
  return static_cast<uintT>(0xffu & value);
}

///
/// \param value
/// \return
template <class uintT>
constexpr inline auto mask16(uintT value) {
  static_assert(std::is_unsigned_v<uintT>, "unsigned integral types only");
  return static_cast<uintT>(0xffffu & value);
}

///
/// \param octet
/// \return
constexpr inline auto is_trail(uint8_t octet) {
  return ((mask8(octet) >> 6u) == 0x2u);
}

///
/// \param code_point
/// \return
constexpr inline auto is_lead_surrogate(char16_t code_point) {
  return (code_point >= constants::surrogates::lead_min) && (code_point <= constants::surrogates::lead_max);
}

///
/// \param value
/// \return
constexpr inline auto is_trail_surrogate(char16_t value) {
  return (value >= constants::surrogates::trail_min) && (value <= constants::surrogates::trail_max);
}

///
/// \param value
/// \return
constexpr inline auto is_surrogate(char16_t value) {
  return (value >= constants::surrogates::lead_min) && (value <= constants::surrogates::trail_max);
}

/// Tests if the code point is a valid value.
/// \param code_point
/// \return \c true if it has a valid value, \c false otherwise
constexpr inline auto is_valid_code_point(char32_t code_point) {
  return (code_point <= constants::code_points::max) && !is_surrogate(static_cast<char16_t>(code_point));
}

/// Returns the size of the sequnce given the lead octet value.
/// \param lead_value
/// \return 1, 2, 3 or 4
constexpr inline auto sequence_length(uint8_t lead_value) {
  auto lead = mask8(lead_value);
  if (lead < 0x80u) {
    return 1;
  } else if ((lead >> 5u) == 0x6u) {
    return 2;
  } else if ((lead >> 4u) == 0xeu) {
    return 3;
  } else if ((lead >> 3u) == 0x1eu) {
    return 4;
  }
  return 0;
}

/// A type used to extract a code point value from an octet sequence
/// \tparam OctetIterator
template <typename OctetIterator>
struct sequence_state {
  constexpr sequence_state(OctetIterator it, char32_t value) : it(it), value(value) {}

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
template <class OctetIterator>
constexpr inline auto make_state(OctetIterator it) -> tl::expected<sequence_state<OctetIterator>, unicode_errc> {
  return sequence_state<OctetIterator>(it, 0);
}

/// Updates the value in the sequence state
///
/// \tparam OctetIterator
/// \param state The input state
/// \param value The updated value
/// \return A new state with an updateds value
template <class OctetIterator>
constexpr inline auto update_value(sequence_state<OctetIterator> state, char32_t value)
    -> sequence_state<OctetIterator> {
  return {state.it, value};
}

/// Moves the octet iterator one character ahead
/// \tparam OctetIterator
/// \param state The input state
/// \return The new state with the updated iterator, on an error if
///         the sequence isn't valid
template <typename OctetIterator>
constexpr inline auto increment(sequence_state<OctetIterator> state)
    -> tl::expected<sequence_state<OctetIterator>, unicode_errc> {
  ++state.it;
  if (!is_trail(*state.it)) {
    return tl::make_unexpected(unicode_errc::illegal_byte_sequence);
  }
  return state;
}

namespace details {
///
/// \tparam OctetIterator
/// \param state
/// \return
template <typename OctetIterator>
constexpr inline auto mask_byte(sequence_state<OctetIterator> state)
    -> tl::expected<sequence_state<OctetIterator>, unicode_errc> {
  return update_value(state, static_cast<char32_t>(mask8(static_cast<std::uint8_t>(*state.it))));
}

/// Converts a two byte code octet sequence to a code point value.
///
/// \tparam OctetIterator
/// \param first
/// \return
template <typename OctetIterator>
constexpr auto from_two_byte_sequence(OctetIterator first)
    -> tl::expected<sequence_state<OctetIterator>, unicode_errc> {
  using result_type = tl::expected<sequence_state<OctetIterator>, unicode_errc>;

  constexpr auto set_code_point = [](auto state) -> result_type {
    return update_value(state, ((state.value << 6) & 0x7ff) + (*state.it & 0x3f));
  };

  return make_state(first)
      .and_then(mask_byte<OctetIterator>)
      .and_then(increment<OctetIterator>)
      .and_then(set_code_point);
}

/// Converts a three byte code octet sequence to a code point value.
///
/// \tparam OctetIterator
/// \param first
/// \return
template <typename OctetIterator>
constexpr inline auto from_three_byte_sequence(OctetIterator first)
    -> tl::expected<sequence_state<OctetIterator>, unicode_errc> {
  using result_type = tl::expected<sequence_state<OctetIterator>, unicode_errc>;

  constexpr auto update_code_point_from_second_byte = [](auto state) -> result_type {
    return update_value(state,
                        ((state.value << 12) & 0xffff) + ((mask8(static_cast<std::uint8_t>(*state.it)) << 6) & 0xfff));
  };

  constexpr auto set_code_point = [](auto state) -> result_type {
    return update_value(state, state.value + (*state.it & 0x3f));
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
template <typename OctetIterator>
constexpr inline auto from_four_byte_sequence(OctetIterator first)
    -> tl::expected<sequence_state<OctetIterator>, unicode_errc> {
  using result_type = tl::expected<sequence_state<OctetIterator>, unicode_errc>;

  constexpr auto update_code_point_from_second_byte = [](auto state) -> result_type {
    return update_value(
        state, ((state.value << 18) & 0x1fffff) + ((mask8(static_cast<std::uint8_t>(*state.it)) << 12) & 0x3ffff));
  };

  constexpr auto update_code_point_from_third_byte = [](auto state) -> result_type {
    return update_value(state, state.value + ((mask8(static_cast<std::uint8_t>(*state.it)) << 6) & 0xfff));
  };

  constexpr auto set_code_point = [](auto state) -> result_type {
    return update_value(state, state.value + (*state.it & 0x3f));
  };

  return make_state(first)
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
constexpr inline auto find_code_point(OctetIterator first)
    -> tl::expected<sequence_state<OctetIterator>, unicode_errc> {
  const auto length = sequence_length(*first);
  switch (length) {
    case 1:
      return make_state(first).and_then(details::mask_byte<OctetIterator>);
    case 2:
      return details::from_two_byte_sequence(first);
    case 3:
      return details::from_three_byte_sequence(first);
    case 4:
      return details::from_four_byte_sequence(first);
    default:
      return tl::make_unexpected(unicode_errc::overflow);
  }
}
}  // namespace skyr::inline v2::unicode

#endif  // SKYR_V2_UNICODE_CORE_HPP
