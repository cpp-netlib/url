// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "skyr/url/filesystem.hpp"
#include "skyr/url/percent_encoding/percent_decode_range.hpp"

namespace skyr::filesystem {
namespace {
class path_error_category : public std::error_category {
 public:
  [[nodiscard]] const char *name() const noexcept override;
  [[nodiscard]] std::string message(int error) const noexcept override;
};

const char *path_error_category::name() const noexcept {
  return "filesystem path";
}

std::string path_error_category::message(int error) const noexcept {
  switch (static_cast<path_errc>(error)) {
    case path_errc::invalid_path:
      return "Invalid path";
    case path_errc::percent_decoding_error:
      return "Percent decoding error";
    default:
      return "(Unknown error)";
  }
}

const path_error_category category{};
}  // namespace

std::error_code make_error_code(path_errc error) {
  return std::error_code(static_cast<int>(error), category);
}

tl::expected<url, std::error_code> from_path(const std::filesystem::path &path) {
  url result;
  if (!result.set_protocol("file")) {
    return tl::make_unexpected(make_error_code(path_errc::invalid_path));
  }
  if (!result.set_pathname(path.string())) {
    return tl::make_unexpected(make_error_code(path_errc::invalid_path));
  }
  return result;
}

tl::expected<std::filesystem::path, std::error_code> to_path(const url &input) {
  auto pathname = input.pathname();
  auto decoded = skyr::percent_encoding::as<std::string>(pathname | skyr::percent_encoding::view::decode);
  if (!decoded) {
    return tl::make_unexpected(make_error_code(path_errc::percent_decoding_error));
  }
  return std::filesystem::u8path(decoded.value());
}
}  // namespace skyr::filesystem
