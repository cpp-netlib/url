// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/filesystem/path.hpp>
#include <skyr/percent_encoding/percent_decode_range.hpp>

namespace skyr {
inline namespace v1 {
namespace filesystem {
namespace {
class path_error_category : public std::error_category {
 public:
  [[nodiscard]] const char *name() const noexcept override {
    return "url filesystem path";
  }

  [[nodiscard]] std::string message(int error) const noexcept override {
    switch (static_cast<path_errc>(error)) {
      case path_errc::invalid_path: return "Invalid path";
      case path_errc::percent_decoding_error: return "Percent decoding error";
      default: return "(Unknown error)";
    }
  }
};

const path_error_category category{};
}  // namespace

std::error_code make_error_code(path_errc error) noexcept {
  return std::error_code(static_cast<int>(error), category);
}

tl::expected<url, std::error_code> from_path(const stdfs::path &path) {
  return make_url("file://" + path.generic_u8string());
}

tl::expected<stdfs::path, std::error_code> to_path(const url &input) {
  auto pathname = input.pathname();
  auto decoded = skyr::percent_encoding::as<std::string>(
      pathname | skyr::percent_encoding::view::decode);
  if (!decoded) {
    return tl::make_unexpected(
        make_error_code(path_errc::percent_decoding_error));
  }
  return stdfs::path(decoded.value());
}
}  // namespace filesystem
}  // namespace v1
}  // namespace skyr
