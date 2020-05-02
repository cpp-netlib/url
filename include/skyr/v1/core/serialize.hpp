// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_SERIALIZE_HPP
#define SKYR_V1_CORE_SERIALIZE_HPP

#include <skyr/v1/core/url_record.hpp>
#include <skyr/v1/core/errors.hpp>

namespace skyr {
inline namespace v1 {
/// Serializes a URL record according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
///
/// \param url A URL record
/// \param exclude_fragment A flag to exclude the fragment from
///        serialization, if set
/// \returns A serialized URL string
auto serialize(
    const url_record &url, bool exclude_fragment=false) -> url_record::string_type;
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_SERIALIZE_HPP
