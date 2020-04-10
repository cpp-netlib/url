// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_SERIALIZE_INC
#define SKYR_URL_SERIALIZE_INC

#include <skyr/core/url_record.hpp>

namespace skyr {
inline namespace v1 {
/// Serializes a URL record according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
///
/// \param url A URL record
/// \param exclude_fragment A flag to exclude the fragment from
///        serialization, if set
/// \returns A serialized URL string
url_record::string_type serialize(
    const url_record &url, bool exclude_fragment = false);
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_URL_SERIALIZE_INC
