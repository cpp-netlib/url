// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_CORE_PARSE_QUERY_HPP
#define SKYR_V2_CORE_PARSE_QUERY_HPP

#include <string>
#include <vector>
#include <skyr/v2/core/parse.hpp>

namespace skyr::inline v2 {
///
/// \param path
/// \param validation_error
/// \return
inline auto parse_path(
    std::string_view path, bool *validation_error) -> tl::expected<std::vector<std::string>, url_parse_errc> {
  auto url = details::basic_parse(path, validation_error, nullptr, nullptr, url_parse_state::path_start);
  if (url) {
    return std::move(url.value()).path;
  }
  return tl::make_unexpected(url.error());
}

///
/// \param path
/// \return
inline auto parse_path(
    std::string_view path) -> tl::expected<std::vector<std::string>, url_parse_errc> {
  [[maybe_unused]] bool validation_error = false;
  return parse_path(path, &validation_error);
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_CORE_PARSE_QUERY_HPP
