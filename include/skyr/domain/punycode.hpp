// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_DOMAIN_PUNYCODE_HPP
#define SKYR_DOMAIN_PUNYCODE_HPP

#include <string>
#include <string_view>
#include <system_error>
#include <tl/expected.hpp>

namespace skyr {
inline namespace v1 {
/// Performs Punycode encoding based on a reference implementation
/// defined in [RFC 3492](https://tools.ietf.org/html/rfc3492)
///
/// \param input A UTF-8 encoded domain to be encoded
/// \returns The encoded ASCII domain, or an error
auto punycode_encode(
    std::string_view input) -> tl::expected<std::string, std::error_code>;

/// Performs Punycode encoding based on a reference implementation
/// defined in [RFC 3492](https://tools.ietf.org/html/rfc3492)
///
/// \param input A UTF-32 encoded domain to be encoded
/// \returns The encoded ASCII domain, or an error
auto punycode_encode(
    std::u32string_view input) -> tl::expected<std::string, std::error_code>;

/// Performs Punycode decoding based on a reference implementation
/// defined in [RFC 3492](https://tools.ietf.org/html/rfc3492)
///
/// \param input An ASCII encoded domain to be decoded
/// \returns The decoded UTF-8 domain, or an error
auto punycode_decode(
    std::string_view input) -> tl::expected<std::string, std::error_code>;
}  // namespace v1
}  // namespace skyr

#endif // SKYR_DOMAIN_PUNYCODE_HPP
