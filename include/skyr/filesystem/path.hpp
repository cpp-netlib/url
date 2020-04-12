// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_FILESYSTEM_PATH_HPP
#define SKYR_FILESYSTEM_PATH_HPP


#if !defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)
#if !defined(__clang__) && (defined(__GNUC__) && __GNUC__ < 8)
#define SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM
#endif  // !defined(__clang__) && (defined(__GNUC__) && __GNUC__ < 8)
#endif  // !defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)


#if defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)
#define _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM
#define SKYR_FILESYSTEM_HEADER() <experimental/filesystem>
#define SKYR_DEFINE_FILESYSTEM_NAMESPACE_ALIAS(name) \
namespace name = std::experimental::filesystem;
#else
#define SKYR_FILESYSTEM_HEADER() <filesystem>
#define SKYR_DEFINE_FILESYSTEM_NAMESPACE_ALIAS(name) \
namespace name = std::filesystem;
#endif // defined(SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)


#include <system_error>
#include SKYR_FILESYSTEM_HEADER()
#include <tl/expected.hpp>
#include <skyr/url.hpp>

namespace skyr {
inline namespace v1 {
/// \namespace filesystem
/// Contains functions to convert from filesystem path to URLs and
/// vice versa
namespace filesystem {
SKYR_DEFINE_FILESYSTEM_NAMESPACE_ALIAS(stdfs)

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
auto make_error_code(path_errc error) noexcept -> std::error_code;

/// Converts a path object to a URL with a file protocol. Handles
/// some processing, including percent encoding
/// \param path A filesystem path
/// \returns a url object or an error on failure
auto from_path(const stdfs::path &path) -> tl::expected<url, std::error_code>;

/// Converts a URL pathname to a filesystem path
/// \param input A url object
/// \returns a path object or an error on failure
auto to_path(const url &input) -> tl::expected<stdfs::path, std::error_code>;
}  // namespace filesystem
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_FILESYSTEM_PATH_HPP
