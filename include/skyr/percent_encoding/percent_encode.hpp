// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PERCENT_ENCODE_HPP
#define SKYR_URL_PERCENT_ENCODE_HPP

#include <string_view>
#include <tl/expected.hpp>
#include <skyr/percent_encoding/percent_encode_range.hpp>
#include <skyr/percent_encoding/errors.hpp>

namespace skyr {
inline namespace v1 {
/// Percent encodes the input
/// \returns The percent encoded output when successful, an error otherwise.
template <class Output>
inline auto percent_encode(std::string_view input) {
  using namespace v1::percent_encoding;
  return as<Output>(input | view::encode);
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_PERCENT_ENCODE_HPP
