// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_CORE_SERIALIZE_HPP
#define SKYR_CORE_SERIALIZE_HPP

#include <format>
#include <ranges>  // was: range/v3/view/join.hpp>
#include <skyr/core/url_record.hpp>

namespace skyr {
namespace details {
inline auto serialize_password(const url_record &url) -> std::string {
  return !url.password.empty() ? std::format(":{}", url.password) : std::string{};
}

inline auto serialize_credentials(const url_record &url) -> std::string {
  return url.includes_credentials() ? std::format("{}{}@", url.username, serialize_password(url)) : std::string{};
}

inline auto serialize_port(const url_record &url) -> std::string {
  return url.port ? std::format(":{}", url.port.value()) : std::string{};
}

inline auto serialize_file_scheme(const url_record &url) -> std::string {
  return (!url.host && (url.scheme == "file")) ? "//" : "";
}

inline auto serialize_authority(const url_record &url) -> std::string {
  return url.host
             ? std::format("//{}{}{}", serialize_credentials(url), url.host.value().serialize(), serialize_port(url))
             : serialize_file_scheme(url);
}

inline auto serialize_path(const std::vector<std::string> &path) -> std::string {
  // Pre-calculate total size: one '/' per segment plus segment lengths
  auto total_size = path.size();  // For the '/' characters
  for (const auto &segment : path) {
    total_size += segment.size();
  }

  std::string result;
  result.reserve(total_size);
  for (auto it = path.begin(); it != path.end(); ++it) {
    result.push_back('/');
    result.append(*it);
  }
  return result;
}

inline auto serialize_path(const url_record &url) -> std::string {
  return url.cannot_be_a_base_url ? url.path.front() : serialize_path(url.path);
}

inline auto serialize_query(const url_record &url) -> std::string {
  return url.query ? std::format("?{}", url.query.value()) : std::string{};
}

inline auto serialize_fragment(const url_record &url) -> std::string {
  return url.fragment ? std::format("#{}", url.fragment.value()) : std::string{};
}
}  // namespace details

/// Serializes a URL record according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
///
/// \param url A URL record
/// \returns A serialized URL string, excluding the fragment
inline auto serialize_excluding_fragment(const url_record &url) -> url_record::string_type {
  return std::format("{}:{}{}{}", url.scheme, details::serialize_authority(url), details::serialize_path(url),
                     details::serialize_query(url));
}

/// Serializes a URL record according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
///
/// \param url A URL record
/// \returns A serialized URL string
inline auto serialize(const url_record &url) -> url_record::string_type {
  return std::format("{}{}", serialize_excluding_fragment(url), details::serialize_fragment(url));
}
}  // namespace skyr

#endif  // SKYR_CORE_SERIALIZE_HPP
