// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_PERCENT_ENCODING_PERCENT_ENCODE_HPP
#define SKYR_PERCENT_ENCODING_PERCENT_ENCODE_HPP

#include <string_view>
#include <string>
#include <ranges>
#include <skyr/percent_encoding/percent_encoded_char.hpp>

namespace skyr {
/// Percent encodes the input
/// \returns The percent encoded output when successful, an error otherwise.
inline auto percent_encode_bytes(std::string_view input, percent_encoding::encode_set encodes) -> std::string {
  auto encode = [encodes](auto byte) {
    using percent_encoding::percent_encode_byte;
    return percent_encode_byte(std::byte(byte), encodes);
  };

  auto result = std::string{};
  result.reserve(input.size() * 3);  // Worst case: each byte becomes "%XX"
  for (const auto& encoded : input | std::ranges::views::transform(encode)) {
    result.append(encoded.begin(), encoded.end());
  }
  return result;
}

inline auto percent_encode(std::string_view input) -> std::string {
  return percent_encode_bytes(input, percent_encoding::encode_set::component);
}
}  // namespace skyr

#endif  // SKYR_PERCENT_ENCODING_PERCENT_ENCODE_HPP
