// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_FILESYSTEM_PATH_HPP
#define SKYR_FILESYSTEM_PATH_HPP

#include <system_error>
#include <filesystem>
#include <tl/expected.hpp>
#include <skyr/url.hpp>

namespace skyr {
inline namespace v1 {
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

/// Creates a `std::error_code` given a `skyr::path_errc` value
/// \param error A filesystem path conversion error
/// \returns A `std::error_code` object
std::error_code make_error_code(path_errc error) noexcept;

/// Converts a path object to a URL with a file protocol. Handles
/// some processing, including percent encoding
/// \param path A filesystem path
/// \returns a url object or an error on failure
tl::expected<url, std::error_code> from_path(const std::filesystem::path &path);

/// Converts a URL pathname to a filesystem path
/// \param input A url object
/// \returns a path object or an error on failure
tl::expected<std::filesystem::path, std::error_code> to_path(const url &input);
}  // namespace filesystem
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_FILESYSTEM_PATH_HPP
