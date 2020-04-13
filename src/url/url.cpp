// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <functional>
#include <skyr/url.hpp>
#include <skyr/core/parse.hpp>
#include <skyr/core/serialize.hpp>
#include <skyr/percent_encoding/percent_encode_range.hpp>
#include <skyr/domain/domain.hpp>
#include "core/url_parse_impl.hpp"
#include "skyr/core/schemes.hpp"

namespace skyr {
inline namespace v1 {
void url::swap(url &other) noexcept {
  using std::swap;
  swap(url_, other.url_);
  swap(href_, other.href_);
  view_ = string_view(href_);
  other.view_ = string_view(other.href_);
  swap(parameters_, other.parameters_);
}

void url::initialize(string_view input, std::optional<url_record> &&base) {
  using result_type = tl::expected<void, std::error_code>;

  parse(input, base)
      .and_then([=](auto &&url) -> result_type {
        update_record(std::forward<url_record>(url));
        return {};
      })
      .or_else([](auto &&error) -> result_type {
        SKYR_EXCEPTIONS_THROW(url_parse_error(error));
        return {};
      });
}

void url::update_record(url_record &&url) {
  url_ = url;
  href_ = serialize(url_);
  view_ = string_view(href_);
  parameters_.initialize(
      url_.query ? string_view(url_.query.value()) : string_view{});
}

auto url::href() const -> string_type {
  return href_;
}

auto url::set_href(string_view href) -> std::error_code {
  auto new_url = details::basic_parse(href);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::to_json() const -> string_type {
  return href_;
}

auto url::origin() const -> string_type {
  if (url_.scheme == "blob") {
    auto url = details::make_url(pathname(), std::nullopt);
    return url ? url.value().origin() : "";
  } else if ((url_.scheme == "ftp") ||
      (url_.scheme == "http") ||
      (url_.scheme == "https") ||
      (url_.scheme == "ws") ||
      (url_.scheme == "wss")) {
    return protocol() + "//" + host();
  } else if (url_.scheme == "file") {
    return "";
  }
  return "null";
}

auto url::protocol() const -> string_type {
  return url_.scheme + ":";
}

auto url::set_protocol(string_view protocol) -> std::error_code {
  auto protocol_ = static_cast<string_type>(protocol);
  if (protocol_.back() != ':') {
    protocol_ += ':';
    protocol = string_view(protocol_);
  }

  auto new_url = details::basic_parse(
      protocol, std::nullopt, url_, url_parse_state::scheme_start);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::username() const -> string_type {
  return url_.username;
}

auto url::set_username(string_view username) -> std::error_code {
  if (url_.cannot_have_a_username_password_or_port()) {
    return make_error_code(
        url_parse_errc::cannot_have_a_username_password_or_port);
  }

  auto new_url = url_;

  new_url.username.clear();
  for (auto c : username) {
    auto pct_encoded = percent_encode_byte(c, percent_encoding::encode_set::userinfo);
    new_url.username += pct_encoded.to_string();
  }

  update_record(std::move(new_url));
  return {};
}

auto url::password() const -> string_type {
  return url_.password;
}

auto url::set_password(string_view password) -> std::error_code {
  if (url_.cannot_have_a_username_password_or_port()) {
    return make_error_code(
        url_parse_errc::cannot_have_a_username_password_or_port);
  }

  auto new_url = url_;

  new_url.password.clear();
  for (auto c : password) {
    auto pct_encoded = percent_encode_byte(c, percent_encoding::encode_set::userinfo);
    new_url.password += pct_encoded.to_string();
  }

  update_record(std::move(new_url));
  return {};
}

auto url::host() const -> url::string_type {
  if (!url_.host) {
    return {};
  }

  if (!url_.port) {
    return url_.host.value();
  }

  return url_.host.value() + ":" + std::to_string(url_.port.value());
}

auto url::set_host(string_view host) -> std::error_code {
  if (url_.cannot_be_a_base_url) {
    return make_error_code(
        url_parse_errc::cannot_be_a_base_url);
  }

  auto new_url = details::basic_parse(
      host, std::nullopt, url_, url_parse_state::host);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::hostname() const -> string_type {
  if (!url_.host) {
    return {};
  }

  return url_.host.value();
}

auto url::set_hostname(string_view hostname) -> std::error_code {
  if (url_.cannot_be_a_base_url) {
    return make_error_code(
        url_parse_errc::cannot_be_a_base_url);
  }

  auto new_url = details::basic_parse(
      hostname, std::nullopt, url_, url_parse_state::hostname);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::is_ipv4_address() const -> bool {
  return parse_ipv4_address(hostname()).has_value();
}

auto url::ipv4_address() const -> std::optional<skyr::ipv4_address> {
  auto address = parse_ipv4_address(hostname());
  return address ? std::make_optional(address.value()) : std::nullopt;
}

auto url::is_ipv6_address() const -> bool {
  if (!url_.host) {
    return false;
  }
  auto view = std::string_view(url_.host.value());
  if ((view.size() <= 2) || view.front() != '[' || view.back() != ']') {
    return false;
  }
  return parse_ipv6_address(view.substr(1, view.size() - 2)).has_value();
}

auto url::ipv6_address() const -> std::optional<skyr::ipv6_address> {
  if (!url_.host) {
    return std::nullopt;
  }
  auto view = std::string_view(url_.host.value());
  if ((view.size() <= 2) || view.front() != '[' || view.back() != ']') {
    return std::nullopt;
  }

  auto address = parse_ipv6_address(view.substr(1, view.size() - 2));
  return address.has_value() ? std::make_optional(address.value()) : std::nullopt;
}

auto url::is_domain() const -> bool {
  return url_.is_special() && !hostname().empty() && !is_ipv4_address() && !is_ipv6_address();
}

[[nodiscard]] auto url::domain() const -> std::optional<string_type> {
  return is_domain()? std::make_optional(skyr::domain_to_unicode(hostname()).value()) : std::nullopt;
}

auto url::is_opaque() const -> bool {
  return !url_.is_special() && !hostname().empty();
}

auto url::port() const -> string_type {
  if (!url_.port) {
    return {};
  }

  return std::to_string(url_.port.value());
}

auto url::set_port(string_view port) -> std::error_code {
  if (url_.cannot_have_a_username_password_or_port()) {
    return make_error_code(
        url_parse_errc::cannot_have_a_username_password_or_port);
  }

  if (port.empty()) {
    auto new_url = url_;
    new_url.port = std::nullopt;
    update_record(std::move(new_url));
  } else {
    auto new_url = details::basic_parse(
        port, std::nullopt, url_, url_parse_state::port);
    if (!new_url) {
      return new_url.error();
    }
    update_record(std::move(new_url).value());
  }

  return {};
}

auto url::pathname() const -> string_type {
  if (url_.cannot_be_a_base_url) {
    return url_.path.front();
  }

  if (url_.path.empty()) {
    return {};
  }

  auto pathname = string_type("/");
  for (const auto &segment : url_.path) {
    pathname += segment;
    pathname += "/";
  }
  return pathname.substr(0, pathname.length() - 1);
}

auto  url::set_pathname(string_view pathname) -> std::error_code {
  if (url_.cannot_be_a_base_url) {
    return make_error_code(
        url_parse_errc::cannot_be_a_base_url);
  }

  url_.path.clear();
  auto new_url = details::basic_parse(
      pathname, std::nullopt, url_, url_parse_state::path_start);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::search() const -> string_type {
  if (!url_.query || url_.query.value().empty()) {
    return {};
  }

  return "?" + url_.query.value();
}

auto url::set_search(string_view search) -> std::error_code {
  auto url = url_;
  if (search.empty()) {
    url.query = std::nullopt;
    update_record(std::move(url));
    return {};
  }

  if (search.front() == '?') {
    search.remove_prefix(1);
  }

  url.query = "";
  auto new_url = details::basic_parse(
      search, std::nullopt, url, url_parse_state::query);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::search_parameters() -> url_search_parameters & {
  return parameters_;
}

auto url::search_parameters() const -> const url_search_parameters & {
  return parameters_;
}

auto url::hash() const -> string_type {
  if (!url_.fragment || url_.fragment.value().empty()) {
    return {};
  }

  return "#" + url_.fragment.value();
}

auto url::set_hash(string_view hash) -> std::error_code {
  if (hash.empty()) {
    url_.fragment = std::nullopt;
    update_record(std::move(url_));
    return {};
  }

  if (hash.front() == '#') {
    hash.remove_prefix(1);
  }

  url_.fragment = "";
  auto new_url = details::basic_parse(hash, std::nullopt, url_, url_parse_state::fragment);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::default_port(std::string_view scheme) noexcept -> std::optional<std::uint16_t> {
  return skyr::default_port(scheme);
}

void url::clear() {
  update_record(url_record{});
}

void swap(url &lhs, url &rhs) noexcept {
  lhs.swap(rhs);
}

namespace details {
auto make_url(
    url::string_view input,
    const std::optional<url_record> &base) -> tl::expected<url, std::error_code> {
  return parse(input, base)
      .and_then([](auto &&new_url) -> tl::expected<url, std::error_code> {
        return url(std::forward<url_record>(new_url));
      });
}
}  // namespace details
}  // namespace v1
}  // namespace skyr
