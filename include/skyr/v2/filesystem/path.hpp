// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_FILESYSTEM_PATH_HPP
#define SKYR_V2_FILESYSTEM_PATH_HPP

#include <filesystem>
#include <tl/expected.hpp>
#include <skyr/v2/url.hpp>
#include <skyr/v2/percent_encoding/percent_decode.hpp>

namespace skyr {
inline namespace v2 {
/// \namespace filesystem
/// Contains functions to convert from filesystem path to URLs and
/// vice versa
namespace filesystem {
///
enum class path_errc {
  ///
  invalid_path = 1,
  ///
  percent_decoding_error,
};

/// Converts a path object to a URL with a file protocol. Handles
/// some processing, including percent encoding
/// \param path A filesystem path
/// \returns a url object or an error on failure
inline auto from_path(const std::filesystem::path &path) -> tl::expected<url, url_parse_errc> {
  return make_url("file://" + path.generic_u8string());
}

/// Converts a URL pathname to a filesystem path
/// \param input A url object
/// \returns a path object or an error on failure
inline auto to_path(const url &input) -> tl::expected<std::filesystem::path, path_errc> {
  auto pathname = input.pathname();
  auto decoded = skyr::percent_decode(pathname);
  if (!decoded) {
    return tl::make_unexpected(path_errc::percent_decoding_error);
  }
  return std::filesystem::path(decoded.value());
}
}  // namespace filesystem
}  // namespace v2
}  // namespace skyr

#endif  // SKYR_V2_FILESYSTEM_PATH_HPP
