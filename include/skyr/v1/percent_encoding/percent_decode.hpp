// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_PERCENT_DECODING_PERCENT_DECODE_HPP
#define SKYR_V1_PERCENT_DECODING_PERCENT_DECODE_HPP

#include <string_view>
#include <tl/expected.hpp>
#include <skyr/v1/percent_encoding/percent_decode_range.hpp>
#include <skyr/v1/percent_encoding/errors.hpp>

namespace skyr {
inline namespace v1 {
/// Percent decodes the input
/// \returns The percent decoded output when successful, an error otherwise.
inline auto percent_decode(std::string_view input) -> tl::expected<std::string, percent_encoding::percent_encode_errc> {
  auto result = std::string{};

  auto range = percent_encoding::percent_decode_range{input};
  for (auto it = std::cbegin(range); it != std::cend(range); ++it) {
    if (!*it) {
      return tl::make_unexpected((*it).error());
    }
    result.push_back((*it).value());
  }
  return result;
}
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_PERCENT_DECODING_PERCENT_DECODE_HPP
