// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PARSE_INC
#define SKYR_URL_PARSE_INC

#include <system_error>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/url/url_record.hpp>

namespace skyr {
/// Parses a URL and returns a `url_record`
///
/// \param input The input string
/// \param base An optional base URL
/// \returns A `url_record` on success and an error code on failure
tl::expected<url_record, std::error_code> parse(
    url_record::string_type input,
    std::optional<url_record> base = std::nullopt);
}  // namespace skyr

#endif  // SKYR_URL_PARSE_INC
