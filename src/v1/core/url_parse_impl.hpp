// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_URL_PARSE_IMPL_HPP
#define SKYR_V1_URL_PARSE_IMPL_HPP

#include <string>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/v1/core/url_record.hpp>

namespace skyr {
inline namespace v1 {
/// States of the URL parser
enum class url_parse_state {
  /// Pointer is at the scheme start
  scheme_start,
  /// Pointer is at the scheme part
  scheme,
  /// For URLs that have no scheme (this may still be valid)
  no_scheme,
  /// URL may be either relative or authority
  special_relative_or_authority,
  /// URL may be either path or authority
  path_or_authority,
  /// URL is relative and pointer is at the relative part
  relative,
  /// URL is relative and pointer is at the relative part and its
  /// value is a slash
  relative_slash,
  /// URL is an authority and pointer is currently at two slashes
  special_authority_slashes,
  /// Pointer is at the authority part and can ignore slashes
  special_authority_ignore_slashes,
  /// Pointer is at the authority part
  authority,
  /// Pointer is at the host part
  host,
  /// Pointer is at the hostname part
  hostname,
  /// Pointer is at the port part
  port,
  /// URL is a file
  file,
  /// URL is a file and pointer value is a slash
  file_slash,
  /// URL is a file and pointer is at the host part
  file_host,
  /// Pointer is at the start of the path part
  path_start,
  /// Pointer is at the path part
  path,
  /// URL cannot be a base URL
  cannot_be_a_base_url_path,
  /// Pointer is at the query part
  query,
  /// Pointer is at the fragment part
  fragment,
};

namespace details {
/// \param input The input string that will be parsed
/// \param validation_error This value is set if there was a
///                         validation error during parsing
/// \param base An optional base URL
/// \param url An optional `url_record`
/// \param state_override An optional parameter to override the
///                       parser state
/// \returns A `url_record` on success and an error code on failure
auto basic_parse(
    std::string_view input,
    bool *validation_error,
    const url_record *base,
    const url_record *url,
    std::optional<url_parse_state> state_override) -> tl::expected<url_record, url_parse_errc>;

inline auto parse(
    std::string_view input,
    bool *validation_error,
    const url_record *base) -> tl::expected<url_record, url_parse_errc> {
  auto url = basic_parse(input, validation_error, base, nullptr, std::nullopt);

  if (!url) {
    return url;
  }

  if (url.value().scheme == "blob") {
    return url;
  }

  if (url.value().path.empty()) {
    return url;
  }

  return url;
}

inline auto parse(
    std::string_view input,
    bool *validation_error) -> tl::expected<url_record, url_parse_errc> {
  return parse(input, validation_error, nullptr);
}
}  // namespace details
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_URL_PARSE_IMPL_HPP
