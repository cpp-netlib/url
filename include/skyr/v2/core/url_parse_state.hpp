// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_URL_PARSE_STATE_HPP
#define SKYR_V2_URL_PARSE_STATE_HPP

#include <string>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/v2/core/url_record.hpp>

namespace skyr::inline v2 {
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
}  // namespace skyr::inline v2

#endif  // SKYR_V2_URL_PARSE_STATE_HPP
