// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_PERCENT_DECODING_PERCENT_DECODE_HPP
#define SKYR_PERCENT_DECODING_PERCENT_DECODE_HPP

#include <expected>
#include <string_view>

#include <skyr/percent_encoding/errors.hpp>
#include <skyr/percent_encoding/percent_decode_range.hpp>

namespace skyr {
/// Percent decodes the input
/// \returns The percent decoded output when successful, an error otherwise.
inline auto percent_decode(std::string_view input)
    -> std::expected<std::string, percent_encoding::percent_encode_errc> {
  auto result = std::string{};
  for (auto&& value : percent_encoding::percent_decode_range{input}) {
    if (!value) {
      return std::unexpected((value).error());
    }
    result.push_back((value).value());
  }
  return result;
}
}  // namespace skyr

#endif  // SKYR_PERCENT_DECODING_PERCENT_DECODE_HPP
