// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_PARSE_HPP
#define SKYR_V1_CORE_PARSE_HPP

#include <system_error>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/v1/core/url_record.hpp>
#include <skyr/v1/core/errors.hpp>

namespace skyr {
inline namespace v1 {
/// Parses a URL is according to the
/// [WhatWG specification](https://url.spec.whatwg.org/)
///
/// \param input A UTF-8 encoded input string
/// \returns An expected ``url_record`` on success, or a
///          ``url_parse_errc`` if parsing ``input`` failed
auto parse(
    std::string_view input) -> tl::expected<url_record, url_parse_errc>;

/// Parses a URL is according to the
/// [WhatWG specification](https://url.spec.whatwg.org/)
///
/// \param input A UTF-8 encoded input string
/// \returns An expected ``url_record`` on success, or a
///          ``url_parse_errc`` if parsing ``input`` failed
auto parse(
    std::string_view input,
    bool *validation_error) -> tl::expected<url_record, url_parse_errc>;

/// Parses a URL is according to the
/// [WhatWG specification](https://url.spec.whatwg.org/)
///
/// \param input A UTF-8 encoded input string
/// \param base A base ``url_record``
/// \returns An expected ``url_record`` on success, or a
///          ``url_parse_errc`` if parsing ``input`` failed
auto parse(
    std::string_view input,
    const url_record &base) -> tl::expected<url_record, url_parse_errc>;

/// Parses a URL is according to the
/// [WhatWG specification](https://url.spec.whatwg.org/)
///
/// \param input A UTF-8 encoded input string
/// \param base A base ``url_record``
/// \returns An expected ``url_record`` on success, or a
///          ``url_parse_errc`` if parsing ``input`` failed
auto parse(
    std::string_view input,
    const url_record &base,
    bool *validation_error) -> tl::expected<url_record, url_parse_errc>;
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_PARSE_HPP
