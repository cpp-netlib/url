// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_PERCENT_DECODING_PERCENT_DECODE_HPP
#define SKYR_V2_PERCENT_DECODING_PERCENT_DECODE_HPP

#include <string_view>
#include <tl/expected.hpp>
#include <skyr/v2/percent_encoding/percent_decode_range.hpp>
#include <skyr/v2/percent_encoding/errors.hpp>

namespace skyr::inline v2 {
/// Percent decodes the input
/// \returns The percent decoded output when successful, an error otherwise.
inline auto percent_decode(std::string_view input) -> tl::expected<std::string, percent_encoding::percent_encode_errc> {
  auto result = std::string{};
  for (auto &&value : percent_encoding::percent_decode_range{input}) {
    if (!value) {
      return tl::make_unexpected((value).error());
    }
    result.push_back((value).value());
  }
  return result;
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_PERCENT_DECODING_PERCENT_DECODE_HPP
