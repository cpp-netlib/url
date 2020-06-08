// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_SERIALIZE_HPP
#define SKYR_V1_CORE_SERIALIZE_HPP

#include <skyr/v1/core/url_record.hpp>
#include <skyr/v1/core/errors.hpp>
#include <skyr/v1/network/ipv4_address.hpp>
#include <skyr/v1/network/ipv6_address.hpp>

namespace skyr {
inline namespace v1 {
/// Serializes a URL record according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
///
/// \param url A URL record
/// \returns A serialized URL string, excluding the fragment
inline auto serialize_excluding_fragment(const url_record &url) -> url_record::string_type {
  auto output = url.scheme + ":";

  if (url.host) {
    output += "//";
    if (url.includes_credentials()) {
      output += url.username;
      if (!url.password.empty()) {
        output += ":";
        output += url.password;
      }
      output += "@";
    }

    output += url.host.value().serialize();

    if (url.port) {
      output += ":";
      output += std::to_string(url.port.value());
    }
  } else if (!url.host && (url.scheme == "file")) {
    output += "//";
  }

  if (url.cannot_be_a_base_url) {
    output += url.path.front();
  } else {
    for (const auto &segment : url.path) {
      output += "/";
      output += segment;
    }
  }

  if (url.query) {
    output += "?";
    output += url.query.value();
  }

  return output;
}

/// Serializes a URL record according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
///
/// \param url A URL record
/// \returns A serialized URL string
inline auto serialize(const url_record &url) -> url_record::string_type {
  auto output = serialize_excluding_fragment(url);

  if (url.fragment) {
    output += "#";
    output += url.fragment.value();
  }

  return output;
}
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_SERIALIZE_HPP
