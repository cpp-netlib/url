// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_PARSE_HOST_HPP
#define SKYR_V1_CORE_PARSE_HOST_HPP

#include <variant>
#include <tl/expected.hpp>
#include <skyr/v1/core/errors.hpp>
#include <skyr/v1/network/ipv4_address.hpp>
#include <skyr/v1/network/ipv6_address.hpp>

namespace skyr {
inline namespace v1 {
using host_types = std::variant<
    std::string,
    ipv4_address,
    ipv6_address>;

auto parse_host(
    std::string_view input,
    bool is_not_special,
    bool *validation_error) -> tl::expected<host_types, url_parse_errc>;

auto parse_host(
    std::string_view input,
    bool is_not_special) -> tl::expected<host_types, url_parse_errc>;
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CORE_PARSE_HOST_HPP
