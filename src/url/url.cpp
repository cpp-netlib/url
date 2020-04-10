// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <functional>
#include "skyr/url.hpp"
#include "skyr/url/url_parse.hpp"
#include "skyr/url/url_serialize.hpp"
#include "skyr/url/percent_encoding/percent_encode_range.hpp"
#include "url_parse_impl.hpp"
#include "url_schemes.hpp"

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

url::string_type url::href() const {
  return href_;
}

tl::expected<void, std::error_code> url::set_href(string_view href) {
  return details::basic_parse(href)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

url::string_type url::to_json() const {
  return href_;
}

url::string_type url::origin() const {
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

url::string_type url::protocol() const {
  return url_.scheme + ":";
}

tl::expected<void, std::error_code> url::set_protocol(string_view protocol) {
  auto protocol_ = static_cast<string_type>(protocol);
  if (protocol_.back() != ':') {
    protocol_ += ':';
    protocol = string_view(protocol_);
  }

  return details::basic_parse(
      protocol, std::nullopt, url_, url_parse_state::scheme_start)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

url::string_type url::username() const {
  return url_.username;
}

tl::expected<void, std::error_code> url::set_username(string_view username) {
  if (url_.cannot_have_a_username_password_or_port()) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_have_a_username_password_or_port));
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

url::string_type url::password() const {
  return url_.password;
}

tl::expected<void, std::error_code> url::set_password(string_view password) {
  if (url_.cannot_have_a_username_password_or_port()) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_have_a_username_password_or_port));
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

url::string_type url::host() const {
  if (!url_.host) {
    return {};
  }

  if (!url_.port) {
    return url_.host.value();
  }

  return url_.host.value() + ":" + std::to_string(url_.port.value());
}

tl::expected<void, std::error_code> url::set_host(string_view host) {
  if (url_.cannot_be_a_base_url) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_be_a_base_url));
  }

  return details::basic_parse(
      host, std::nullopt, url_, url_parse_state::host)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

url::string_type url::hostname() const {
  if (!url_.host) {
    return {};
  }

  return url_.host.value();
}

tl::expected<void, std::error_code> url::set_hostname(string_view hostname) {
  if (url_.cannot_be_a_base_url) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_be_a_base_url));
  }

  return details::basic_parse(
      hostname, std::nullopt, url_, url_parse_state::hostname)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

bool url::is_ipv4_address() const {
  return parse_ipv4_address(hostname()).has_value();
}

std::optional<skyr::ipv4_address> url::ipv4_address() const {
  auto address = parse_ipv4_address(hostname());
  return address.has_value() ? std::make_optional(address.value()) : std::nullopt;
}

bool url::is_ipv6_address() const {
  if (!url_.host) {
    return false;
  }
  auto view = std::string_view(url_.host.value());
  if ((view.size() <= 2) || view.front() != '[' || view.back() != ']') {
    return false;
  }
  return parse_ipv6_address(view.substr(1, view.size() - 2)).has_value();
}

std::optional<skyr::ipv6_address> url::ipv6_address() const {
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

bool url::is_domain() const {
  return !hostname().empty() && !is_ipv4_address() && !is_ipv6_address();
}

url::string_type url::port() const {
  if (!url_.port) {
    return {};
  }

  return std::to_string(url_.port.value());
}

tl::expected<void, std::error_code> url::set_port(string_view port) {
  if (url_.cannot_have_a_username_password_or_port()) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_have_a_username_password_or_port));
  }

  if (port.empty()) {
    auto new_url = url_;
    new_url.port = std::nullopt;
    update_record(std::move(new_url));
  } else {
    return details::basic_parse(
        port, std::nullopt, url_, url_parse_state::port)
        .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
          update_record(std::forward<url_record>(new_url));
          return {};
        });
  }

  return {};
}

url::string_type url::pathname() const {
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

tl::expected<void, std::error_code> url::set_pathname(string_view pathname) {
  if (url_.cannot_be_a_base_url) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_be_a_base_url));
  }

  url_.path.clear();
  return details::basic_parse(
      pathname, std::nullopt, url_, url_parse_state::path_start)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

url::string_type url::search() const {
  if (!url_.query || url_.query.value().empty()) {
    return {};
  }

  return "?" + url_.query.value();
}

tl::expected<void, std::error_code> url::set_search(string_view search) {
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
  return details::basic_parse(
      search, std::nullopt, url, url_parse_state::query)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

url_search_parameters &url::search_parameters() {
  return parameters_;
}

url::string_type url::hash() const {
  if (!url_.fragment || url_.fragment.value().empty()) {
    return {};
  }

  return "#" + url_.fragment.value();
}

tl::expected<void, std::error_code> url::set_hash(string_view hash) {
  if (hash.empty()) {
    url_.fragment = std::nullopt;
    update_record(std::move(url_));
    return {};
  }

  if (hash.front() == '#') {
    hash.remove_prefix(1);
  }

  url_.fragment = "";
  return details::basic_parse(hash, std::nullopt, url_,
                              url_parse_state::fragment)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

std::optional<std::uint16_t> url::default_port(std::string_view scheme) noexcept {
  return details::default_port(scheme);
}

void url::clear() {
  update_record(url_record{});
}

void swap(url &lhs, url &rhs) noexcept {
  lhs.swap(rhs);
}

namespace details {
tl::expected<url, std::error_code> make_url(
    url::string_view input,
    const std::optional<url_record> &base) {
  return parse(input, base)
      .and_then([](auto &&new_url) -> tl::expected<url, std::error_code> {
        return url(std::forward<url_record>(new_url));
      });
}
}  // namespace details
}  // namespace v1
}  // namespace skyr
