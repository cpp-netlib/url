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
inline auto percent_encode_bytes(std::string_view input, percent_encoding::encode_set encodes) -> std::string {
  static auto encode = [&encodes] (auto byte) {
    using percent_encoding::percent_encode_byte;
    return percent_encode_byte(std::byte(byte), encodes);
  };

  auto result = std::string{};
  for (const auto &encoded : input | ranges::views::transform(encode)) {
    result += std::string(std::cbegin(encoded), std::cend(encoded));
  }
  return result;
}

inline auto percent_encode(std::string_view input) -> std::string {
  return percent_encode_bytes(input, percent_encoding::encode_set::component);
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_PERCENT_ENCODING_PERCENT_ENCODE_HPP
