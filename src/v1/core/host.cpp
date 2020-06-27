// Copyright 2016-2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <skyr/v1/core/host.hpp>
#include <skyr/v1/core/errors.hpp>
#include <skyr/v1/percent_encoding/percent_encoded_char.hpp>
#include <skyr/v1/percent_encoding/percent_decode.hpp>
#include <skyr/v1/containers/static_vector.hpp>
#include <skyr/v1/domain/domain.hpp>

#if !defined(SKYR_DOMAIN_MAX_DOMAIN_LENGTH)
#define SKYR_DOMAIN_MAX_DOMAIN_LENGTH 253
#endif // !defined(SKYR_DOMAIN_MAX_DOMAIN_LENGTH)

namespace skyr {
inline namespace v1 {
namespace {
constexpr static auto is_forbidden_host_point = [](auto byte) {
  return
      (byte == '\0') || (byte == '\t') || (byte == '\n') || (byte == '\r') || (byte == ' ') || (byte == '#') ||
      (byte == '%') || (byte == '/') || (byte == ':') || (byte == '<') || (byte == '>') || (byte == '?') ||
      (byte == '@') || (byte == '[') || (byte == '\\') || (byte == ']') || (byte == '^');
};

auto parse_opaque_host(std::string_view input,
                       bool *validation_error) -> tl::expected<skyr::v1::opaque_host, url_parse_errc> {
  constexpr static auto is_forbidden = [] (auto byte) -> bool {
    return (byte != '%') && is_forbidden_host_point(byte);
  };

  auto first = begin(input), last = end(input);
  auto it = std::find_if(first, last, is_forbidden);
  if (it != last) {
    *validation_error |= true;
    return tl::make_unexpected(url_parse_errc::forbidden_host_point);
  }

  auto output = std::string();
  for (auto c : input) {
    auto pct_encoded = percent_encode_byte(std::byte(c), percent_encoding::encode_set::c0_control);
    output += pct_encoded.to_string();
  }
  return skyr::v1::opaque_host{output};
}
}  // namespace

auto parse_host(
    std::string_view input, bool is_not_special, bool *validation_error) -> tl::expected<host, url_parse_errc> {
  if (input.empty()) {
    return host{empty_host{}};
  }

  if (input.front() == '[') {
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
      return skyr::v1::host{ipv6_address.value()};
    }
    else {
      return tl::make_unexpected(url_parse_errc::invalid_ipv6_address);
    }
  }

  if (is_not_special) {
    return parse_opaque_host(input, validation_error).and_then(
        [] (auto &&h) -> tl::expected<host, url_parse_errc> { return host{h}; });
  }

  auto domain = static_vector<char, SKYR_DOMAIN_MAX_DOMAIN_LENGTH>{};
  auto range = percent_encoding::percent_decode_range{input};
  for (auto it = std::cbegin(range); it != std::cend(range); ++it) {
    if ((domain.size() == domain.max_size()) || !*it) {
      return tl::make_unexpected(url_parse_errc::cannot_decode_host_point);
    }
    domain.push_back((*it).value());
  }

  auto ascii_domain = std::string{};
  if (!domain_to_ascii(std::string_view(domain.data(), domain.size()), &ascii_domain)) {
    return tl::make_unexpected(url_parse_errc::domain_error);
  }

  auto it = std::find_if(std::cbegin(ascii_domain), std::cend(ascii_domain), is_forbidden_host_point);
  if (it != std::cend(ascii_domain)) {
    *validation_error |= true;
    return tl::make_unexpected(url_parse_errc::domain_error);
  }

  bool ipv4_validation_error = false;
  auto host = parse_ipv4_address(ascii_domain, &ipv4_validation_error);
  if (!host) {
    if (host.error() == ipv4_address_errc::overflow) {
      return tl::make_unexpected(url_parse_errc::invalid_ipv4_address);
    }
    else {
      return skyr::v1::host{skyr::v1::domain{ascii_domain}};
    }
  }
  *validation_error = ipv4_validation_error;
  return skyr::v1::host{host.value()};
}
}  // namespace v1
}  // namespace skyr
