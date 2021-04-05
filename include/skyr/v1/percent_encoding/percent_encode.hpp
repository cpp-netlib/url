// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_PERCENT_ENCODING_PERCENT_ENCODE_HPP
#define SKYR_V1_PERCENT_ENCODING_PERCENT_ENCODE_HPP

#include <string_view>
#include <string>
#include <range/v3/view/transform.hpp>
#include <skyr/v1/percent_encoding/percent_encoded_char.hpp>

namespace skyr {
inline namespace v1 {
/// Percent encodes the input
/// \returns The percent encoded output.
inline auto percent_encode(std::string_view input) -> std::string {
  static constexpr auto encode = [] (auto byte) {
    using percent_encoding::percent_encode_byte;
    return percent_encode_byte(std::byte(byte), percent_encoding::encode_set::component);
  };

  auto result = std::string{};
  for (const auto &encoded : input | ranges::views::transform(encode)) {
    result += std::string(std::cbegin(encoded), std::cend(encoded));
  }
  return result;
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_PERCENT_ENCODING_PERCENT_ENCODE_HPP
