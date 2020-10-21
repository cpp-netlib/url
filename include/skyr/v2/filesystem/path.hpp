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

namespace skyr::inline v2 {
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
  /// This is weird because not every library implementation has transitioned
  /// from changing the return type of generic_u8string to a std::u8string between C++17 and C++20
  using namespace std::string_view_literals;
  constexpr auto scheme = u8"file://"sv;
  auto input = std::u8string(scheme);
  auto u8string = path.generic_u8string(); // Sometimes a std::string and
                                           // sometimes a std::u8string (correct, for C++20)
  input.append(std::cbegin(u8string), std::cend(u8string));
  return make_url(input);
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
}  // namespace skyr::v2

#endif  // SKYR_V2_FILESYSTEM_PATH_HPP
