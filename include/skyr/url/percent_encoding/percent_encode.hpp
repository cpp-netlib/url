// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PERCENT_ENCODE_INC
#define SKYR_URL_PERCENT_ENCODE_INC

#include <skyr/url/percent_encoding/percent_encode_range.hpp>

namespace skyr {
//enum class encode_set {
//  ///
//  c0_control = 1,
//  ///
//  fragment,
//  ///
//  query,
//  ///
//  path,
//  ///
//  userinfo,
//};
//
///// Percent encodes a byte if it is not in the exclude set
///// \param byte The input byte
///// \param excludes The set of code points to exclude when percent
/////        encoding
///// \returns A percent encoded string if `byte` is not in the
/////          exclude set, `byte` as a string otherwise
//std::string percent_encode_byte(
//    char byte, encode_set excludes);
//
///// Percent encodes a string
///// \param input A string of bytes
///// \param excludes The set of code points to exclude when percent
/////        encoding
///// \returns A percent encoded ASCII string, or an error on failure
//tl::expected<std::string, std::error_code> percent_encode(
//    std::string_view input, encode_set excludes);
}  // namespace skyr

#endif  // SKYR_URL_PERCENT_ENCODE_INC
