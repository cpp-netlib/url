// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/core/url_serialize.hpp>

namespace skyr {
inline namespace v1 {
url_record::string_type serialize(
    const url_record &url, bool exclude_fragment) {
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

    // TODO: serialize host
    output += url.host.value();

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

  if (!exclude_fragment && url.fragment) {
    output += "#";
    output += url.fragment.value();
  }

  return output;
}
}  // namespace v1
}  // namespace skyr
