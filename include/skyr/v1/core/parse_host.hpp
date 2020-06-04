// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_PARSE_HOST_HPP
#define SKYR_V1_CORE_PARSE_HOST_HPP

#include <variant>
#include <string>
#include <string_view>
#include <tl/expected.hpp>
#include <skyr/v1/core/errors.hpp>
#include <skyr/v1/network/ipv4_address.hpp>
#include <skyr/v1/network/ipv6_address.hpp>

namespace skyr {
inline namespace v1 {
/// \typedef host_types
/// One of a string (for domains), an IPv4 address or an IPv6 address.
using host_types = std::variant<
    std::string,
    ipv4_address,
    ipv6_address>;

/// Parses a string to either a domain, IPv4 address or IPv6 address according to
/// https://url.spec.whatwg.org/#host-parsing
/// \param input An input string
/// \param is_not_special \c true to process only non-special hosts, \c false otherwise
/// \param validation_error Set to \c true if there was a validation error
/// \return A host as a domain (std::string), ipv4_address or ipv6_address, or an error code
auto parse_host(
    std::string_view input,
    bool is_not_special,
    bool *validation_error) -> tl::expected<host_types, url_parse_errc>;

/// Parses a string to either a domain, IPv4 address or IPv6 address according to
/// https://url.spec.whatwg.org/#host-parsing
/// Parses a string to either a domain, IPv4 address or IPv6 addess
/// \param input An input string
/// \param is_not_special \c true to process only non-special hosts, \c false otherwise
/// \return A host as a domain (std::string), ipv4_address or ipv6_address, or an error code
auto parse_host(
    std::string_view input,
    bool is_not_special) -> tl::expected<host_types, url_parse_errc>;
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_PARSE_HOST_HPP
