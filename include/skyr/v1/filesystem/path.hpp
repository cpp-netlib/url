// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_FILESYSTEM_PATH_HPP
#define SKYR_V1_FILESYSTEM_PATH_HPP

#include <system_error>
#include <tl/expected.hpp>
#include <skyr/url.hpp>
#include <skyr/percent_encoding/percent_decode.hpp>
#include <skyr/v1/filesystem/portability.hpp>

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

namespace details {
class path_error_category : public std::error_category {
 public:
  [[nodiscard]] auto name() const noexcept -> const char * override {
    return "url filesystem path";
  }

  [[nodiscard]] auto message(int error) const noexcept -> std::string override {
    switch (static_cast<path_errc>(error)) {
      case path_errc::invalid_path: return "Invalid path";
      case path_errc::percent_decoding_error: return "Percent decoding error";
      default: return "(Unknown error)";
    }
  }
};
}  // namespace details

/// Creates a `std::error_code` given a `skyr::path_errc` value
/// \param error A filesystem path conversion error
/// \returns A `std::error_code` object
inline auto make_error_code(path_errc error) noexcept -> std::error_code {
  static const details::path_error_category category{};
  return std::error_code(static_cast<int>(error), category);
}

/// Converts a path object to a URL with a file protocol. Handles
/// some processing, including percent encoding
/// \param path A filesystem path
/// \returns a url object or an error on failure
inline auto from_path(const stdfs::path &path) -> tl::expected<url, std::error_code> {
  return make_url("file://" + path.generic_u8string());
}

/// Converts a URL pathname to a filesystem path
/// \param input A url object
/// \returns a path object or an error on failure

inline auto to_path(const url &input) -> tl::expected<stdfs::path, std::error_code> {
  auto pathname = input.pathname();
  auto decoded = skyr::percent_decode<std::string>(pathname);
  if (!decoded) {
    return tl::make_unexpected(
        make_error_code(path_errc::percent_decoding_error));
  }
  return stdfs::path(decoded.value());
}
}  // namespace filesystem
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_FILESYSTEM_PATH_HPP
