// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_PERCENT_ENCODING_PERCENT_ENCODE_HPP
#define SKYR_V2_PERCENT_ENCODING_PERCENT_ENCODE_HPP

#include <string_view>
#include <string>
#include <range/v3/view/transform.hpp>
#include <skyr/v2/percent_encoding/percent_encoded_char.hpp>

namespace skyr::inline v2 {
/// Percent encodes the input
/// \returns The percent encoded output when successful, an error otherwise.
inline auto percent_encode(std::string_view input) {
  using percent_encoding::percent_encoded_char;

  static constexpr auto encode = [](auto byte) {
    if ((byte == '\x2a') || (byte == '\x2d') || (byte == '\x2e') || ((byte >= '\x30') && (byte <= '\x39')) ||
        ((byte >= '\x41') && (byte <= '\x5a')) || (byte == '\x5f') || ((byte >= '\x61') && (byte <= '\x7a'))) {
      return percent_encoded_char(std::byte(byte), percent_encoded_char::no_encode());
    } else if (byte == '\x20') {
      return percent_encoded_char(std::byte('+'), percent_encoded_char::no_encode());
    }
    return percent_encoded_char(std::byte(byte));
  };

  auto result = std::string{};
  for (const auto &encoded : input | ranges::views::transform(encode)) {
    result += std::string(std::cbegin(encoded), std::cend(encoded));
  }
  return result;
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_PERCENT_ENCODING_PERCENT_ENCODE_HPP
