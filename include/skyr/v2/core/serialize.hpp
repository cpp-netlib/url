// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_CORE_SERIALIZE_HPP
#define SKYR_V2_CORE_SERIALIZE_HPP

#include <fmt/format.h>
#include <range/v3/view/join.hpp>
#include <skyr/v2/core/url_record.hpp>

namespace skyr::inline v2 {
namespace details {
inline auto serialize_password(const url_record &url) -> std::string {
  return !url.password.empty() ? fmt::format(":{}", url.password) : std::string{};
}

inline auto serialize_credentials(const url_record &url) -> std::string {
  return url.includes_credentials() ? fmt::format("{}{}@", url.username, serialize_password(url)) : std::string{};
}

inline auto serialize_port(const url_record &url) -> std::string {
  return url.port ? fmt::format(":{}", url.port.value()) : std::string{};
}

inline auto serialize_file_scheme(const url_record &url) -> std::string {
  return (!url.host && (url.scheme == "file")) ? "//" : "";
}

inline auto serialize_host(const url_record &url) -> std::string {
  return url.host
             ? fmt::format("//{}{}{}", serialize_credentials(url), url.host.value().serialize(), serialize_port(url))
             : serialize_file_scheme(url);
}

inline auto serialize_path(const url_record &url) -> std::string {
  return url.cannot_be_a_base_url ? url.path.front()
                                  : fmt::format("/{}", url.path | ranges::views::join('/') | ranges::to<std::string>());
}

inline auto serialize_query(const url_record &url) -> std::string {
  return url.query ? fmt::format("?{}", url.query.value()) : std::string{};
}

inline auto serialize_fragment(const url_record &url) -> std::string {
  return url.fragment ? fmt::format("#{}", url.fragment.value()) : std::string{};
}
}  // namespace details

/// Serializes a URL record according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
///
/// \param url A URL record
/// \returns A serialized URL string, excluding the fragment
inline auto serialize_excluding_fragment(const url_record &url) -> url_record::string_type {
  return fmt::format("{}:{}{}{}", url.scheme, details::serialize_host(url), details::serialize_path(url),
                     details::serialize_query(url));
}

/// Serializes a URL record according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
///
/// \param url A URL record
/// \returns A serialized URL string
inline auto serialize(const url_record &url) -> url_record::string_type {
  return fmt::format("{}{}", serialize_excluding_fragment(url), details::serialize_fragment(url));
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_CORE_SERIALIZE_HPP
