// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/filesystem/path.hpp>
#include <skyr/percent_encoding/percent_decode.hpp>

namespace skyr {
inline namespace v1 {
namespace filesystem {
auto from_path(const stdfs::path &path) -> tl::expected<url, std::error_code> {
  return make_url("file://" + path.generic_u8string());
}

auto to_path(const url &input) -> tl::expected<stdfs::path, std::error_code> {
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
