// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_UNICODE_CORE_HPP
#define SKYR_V1_UNICODE_CORE_HPP

#include <skyr/v1/unicode/constants.hpp>
#include <skyr/v1/unicode/errors.hpp>
#include <string>
#include <tl/expected.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \param octet
/// \return
constexpr auto mask8(uint8_t octet) {
  return static_cast<uint8_t>(0xffu & octet);
}

///
/// \param value
/// \return
constexpr auto mask16(char16_t value) {
  return static_cast<char16_t>(u'\xffff' & value);
}

///
/// \param octet
/// \return
constexpr auto is_trail(uint8_t octet) {
  return ((mask8(octet) >> 6u) == 0x2u);
}

///
/// \param code_point
/// \return
constexpr auto is_lead_surrogate(char16_t code_point) {
  return
      (code_point >= constants::surrogates::lead_min) &&
      (code_point <= constants::surrogates::lead_max);
}

///
/// \param value
/// \return
constexpr auto is_trail_surrogate(char16_t value) {
  return
      (value >= constants::surrogates::trail_min) &&
      (value <= constants::surrogates::trail_max);
}

///
/// \param value
/// \return
constexpr auto is_surrogate(char16_t value) {
  return
      (value >= constants::surrogates::lead_min) &&
      (value <= constants::surrogates::trail_max);
}

/// Tests if the code point is a valid value.
/// \param code_point
/// \return \c true if it has a valid value, \c false otherwise
constexpr auto is_valid_code_point(char32_t code_point) {
  return
      (code_point <= constants::code_points::max) &&
      !is_surrogate(static_cast<char16_t>(code_point));
}

/// Returns the size of the sequnce given the lead octet value.
/// \param lead_value
/// \return 1, 2, 3 or 4
constexpr auto sequence_length(uint8_t lead_value) {
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
inline auto make_state(OctetIterator it) -> tl::expected<sequence_state<OctetIterator>, std::error_code> {
  return sequence_state<OctetIterator>(it, 0);
}

/// Updates the value in the sequence state
///
/// \tparam OctetIterator
/// \param state The input state
/// \param value The updated value
/// \return A new state with an updateds value
template<class OctetIterator>
inline auto update_value(
    sequence_state<OctetIterator> state,
    char32_t value) -> sequence_state<OctetIterator> {
  return {state.it, value};
}

/// Moves the octet iterator one character ahead
/// \tparam OctetIterator
/// \param state The input state
/// \return The new state with the updated iterator, on an error if
///         the sequence isn't valid
template<typename OctetIterator>
inline auto
increment(sequence_state<OctetIterator> state) -> tl::expected<sequence_state<OctetIterator>, std::error_code> {
  ++state.it;
  if (!is_trail(*state.it)) {
    return tl::make_unexpected(
        make_error_code(unicode_errc::illegal_byte_sequence));
  }
  return state;
}

namespace details {
///
/// \tparam OctetIterator
/// \param state
/// \return
template<typename OctetIterator>
inline auto mask_byte(sequence_state<OctetIterator> state) -> tl::expected<sequence_state<OctetIterator>, std::error_code> {
  return update_value(state, mask8(*state.it));
}

/// Converts a two byte code octet sequence to a code point value.
///
/// \tparam OctetIterator
/// \param first
/// \return
template<typename OctetIterator>
auto from_two_byte_sequence(OctetIterator first) -> tl::expected<sequence_state<OctetIterator>, std::error_code> {
  using result_type = tl::expected<sequence_state<OctetIterator>, std::error_code>;

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
auto from_three_byte_sequence(OctetIterator first) -> tl::expected<sequence_state<OctetIterator>, std::error_code> {
  using result_type = tl::expected<sequence_state<OctetIterator>, std::error_code>;

  auto update_code_point_from_second_byte = [](auto state) -> result_type {
    return update_value(
        state,
        ((state.value << 12) & 0xffff) + ((mask8(*state.it) << 6) & 0xfff));
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
auto from_four_byte_sequence(OctetIterator first) -> tl::expected<sequence_state<OctetIterator>, std::error_code> {
  using result_type = tl::expected<sequence_state<OctetIterator>, std::error_code>;

  auto update_code_point_from_second_byte = [](auto state) -> result_type {
    return update_value(
        state,
        ((state.value << 18) & 0x1fffff) + ((mask8(*state.it) << 12) & 0x3ffff));
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
auto find_code_point(
    OctetIterator first) -> tl::expected<sequence_state<OctetIterator>, std::error_code> {
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
      return tl::make_unexpected(make_error_code(unicode_errc::overflow));
  }
}
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_UNICODE_CORE_HPP
