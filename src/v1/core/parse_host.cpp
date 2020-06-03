// Copyright 2016-2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <skyr/v1/core/parse_host.hpp>
#include <skyr/v1/core/errors.hpp>
#include <skyr/v1/percent_encoding/percent_encoded_char.hpp>
#include <skyr/v1/percent_encoding/percent_decode.hpp>
#include <skyr/v1/domain/domain.hpp>

namespace skyr {
inline namespace v1 {
namespace {
auto is_forbidden_host_point(std::string_view::value_type byte) noexcept {
  using namespace std::string_view_literals;
  constexpr auto forbidden = "\0\t\n\r #%/:<>?@[\\]^"sv;
  auto first = begin(forbidden), last = end(forbidden);
  return last != std::find(first, last, byte);
}

auto parse_opaque_host(std::string_view input, bool *validation_error) -> tl::expected<std::string, url_parse_errc> {
  auto first = begin(input), last = end(input);
  auto it = std::find_if(
      first, last, [] (auto byte) -> bool {
        return (byte != '%') && is_forbidden_host_point(byte);
      });
  if (it != last) {
    *validation_error |= true;
    return tl::make_unexpected(url_parse_errc::forbidden_host_point);
  }

  auto output = std::string();
  for (auto c : input) {
    auto pct_encoded = percent_encode_byte(c, percent_encoding::encode_set::c0_control);
    output += pct_encoded.to_string();
  }
  return output;
}
}  // namespace

auto parse_host(
    std::string_view input, bool is_not_special, bool *validation_error) -> tl::expected<host_types, url_parse_errc> {
  if (!input.empty() && (input.front() == '[')) {
    if (input.back() != ']') {
      *validation_error |= true;
      return tl::make_unexpected(url_parse_errc::invalid_ipv6_address);
    }

    auto view = std::string_view(input);
    view.remove_prefix(1);
    view.remove_suffix(1);
    bool ipv6_validation_error = false;
    auto ipv6_address = parse_ipv6_address(view, &ipv6_validation_error);
    if (ipv6_address) {
      *validation_error = ipv6_validation_error;
      return ipv6_address.value();
    }
    else {
      return tl::make_unexpected(url_parse_errc::invalid_ipv6_address);
    }
  }

  if (is_not_special) {
    return parse_opaque_host(input, validation_error);
  }

  auto domain = percent_decode<std::string>(input);
  if (!domain) {
    return tl::make_unexpected(url_parse_errc::cannot_decode_host_point);
  }

  auto ascii_domain = domain_to_ascii(domain.value());
  if (!ascii_domain) {
    return tl::make_unexpected(url_parse_errc::domain_error);
  }

  auto it = std::find_if(
      begin(ascii_domain.value()), end(ascii_domain.value()), is_forbidden_host_point);
  if (it != end(ascii_domain.value())) {
    *validation_error |= true;
    return tl::make_unexpected(url_parse_errc::domain_error);
  }

  bool ipv4_validation_error = false;
  auto host = parse_ipv4_address(ascii_domain.value(), &ipv4_validation_error);
  if (!host) {
    if (host.error() == ipv4_address_errc::overflow) {
      return tl::make_unexpected(url_parse_errc::invalid_ipv4_address);
    }
    else {
      return ascii_domain.value();
    }
  }
  *validation_error = ipv4_validation_error;
  return host.value();
}

auto parse_host(
    std::string_view input,
    bool is_not_special) -> tl::expected<host_types, url_parse_errc> {
  [[maybe_unused]] bool validation_error = false;
  return parse_host(input, is_not_special, &validation_error);
}
}  // namespace v1
}  // namespace skyr
