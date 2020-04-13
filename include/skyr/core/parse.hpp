// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_CORE_URL_PARSE_INC
#define SKYR_CORE_URL_PARSE_INC

#include <system_error>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/core/url_record.hpp>

namespace skyr {
inline namespace v1 {
/// Parses a URL and returns a `url_record`
///
/// \param input The input string
/// \param base An optional base URL
/// \returns A `url_record` on success and an error code on failure
auto parse(
    std::string_view input,
    std::optional<url_record> base=std::nullopt) -> tl::expected<url_record, std::error_code>;
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_CORE_URL_PARSE_INC
