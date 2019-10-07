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
url::url()
  : url_()
  , href_()
  , view_(href_)
  , parameters_(std::make_shared<url_search_parameters>(*this)) {}

url::url(url_record &&input) noexcept
  : url_(input)
  , href_(serialize(url_))
  , view_(href_)
  , parameters_(std::make_shared<url_search_parameters>(*this)) {}

void url::swap(url &other) noexcept {
  using std::swap;
  swap(url_, other.url_);
  swap(href_, other.href_);
  view_ = string_view(href_);
  other.view_ = string_view(other.href_);
  parameters_.swap(other.parameters_);
}

void url::initialize(string_type &&input, std::optional<url_record> &&base) {
  using result_type = tl::expected<void, std::error_code>;

  parse(input, base)
  .and_then([=](auto &&url) -> result_type {
    parameters_ = std::make_shared<url_search_parameters>(*this);
    update_record(std::forward<url_record>(url));
    return {};
  })
  .or_else([](auto &&error) -> result_type {
    SKYR_EXCEPTIONS_THROW(url_parse_error(error));
    return {};
  });
}

void url::update_record(url_record &&record) {
  url_ = record;
  href_ = serialize(url_);
  view_ = string_view(href_);
//  parameters_ = std::make_shared<url_search_parameters>(*this);
  if (url_.query) {
    parameters_->initialize(url_.query.value());
  }

}

url::string_type url::href() const {
  return href_;
}

tl::expected<void, std::error_code> url::set_href(string_type &&href) {
  return details::basic_parse(href)
  .and_then([this] (auto &&new_url) -> tl::expected<void, std::error_code> {
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
    return url? url.value().origin() : "";
  }
  else if ((url_.scheme == "ftp") ||
      (url_.scheme == "gopher") ||
      (url_.scheme == "http") ||
      (url_.scheme == "https") ||
      (url_.scheme == "ws") ||
      (url_.scheme == "wss")) {
    return protocol() + "//" + host();
  }
  else if (url_.scheme == "file") {
    return "";
  }
  return "null";
}

url::string_type url::protocol() const {
  return url_.scheme + ":";
}

tl::expected<void, std::error_code> url::set_protocol(string_type &&protocol) {
  return details::basic_parse(
      protocol + ":", std::nullopt, url_, url_parse_state::scheme_start)
  .and_then([this] (auto &&new_url) -> tl::expected<void, std::error_code> {
    update_record(std::forward<url_record>(new_url));
    return {};
  });
}

url::string_type url::username() const {
  return url_.username;
}

tl::expected<void, std::error_code> url::set_username(string_type &&username) {
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

tl::expected<void, std::error_code> url::set_password(string_type &&password) {
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

tl::expected<void, std::error_code> url::set_host(string_type &&host) {
  if (url_.cannot_be_a_base_url) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_be_a_base_url));
  }

  return details::basic_parse(
      std::move(host), std::nullopt, url_, url_parse_state::host)
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

tl::expected<void, std::error_code> url::set_hostname(string_type &&hostname) {
  if (url_.cannot_be_a_base_url) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_be_a_base_url));
  }

  return details::basic_parse(
      std::move(hostname), std::nullopt, url_, url_parse_state::hostname)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

url::string_type url::port() const {
  if (!url_.port) {
    return {};
  }

  return std::to_string(url_.port.value());
}

tl::expected<void, std::error_code> url::set_port(string_type &&port) {
  if (url_.cannot_have_a_username_password_or_port()) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_have_a_username_password_or_port));
  }

  if (port.empty()) {
    auto new_url = url_;
    new_url.port = std::nullopt;
    update_record(std::move(new_url));
  }
  else {
    return details::basic_parse(
        std::move(port), std::nullopt, url_, url_parse_state::port)
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

tl::expected<void, std::error_code> url::set_pathname(string_type &&pathname) {
  if (url_.cannot_be_a_base_url) {
    return tl::make_unexpected(make_error_code(
        url_parse_errc::cannot_be_a_base_url));
  }

  url_.path.clear();
  return details::basic_parse(
      std::move(pathname),std:: nullopt, url_, url_parse_state::path_start)
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

tl::expected<void, std::error_code> url::set_search(string_type &&search) {
  auto url = url_;
  if (search.empty()) {
    url.query = std::nullopt;
    update_record(std::move(url));
    return {};
  }

  auto input = search;
  if (input.front() == '?') {
    auto first = std::begin(input), last = std::end(input);
    input.assign(first + 1, last);
  }

  url_.query = "";
  return details::basic_parse(
      std::move(input), std::nullopt, url_, url_parse_state::query)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

url_search_parameters &url::search_parameters() {
  return *parameters_;
}

url::string_type url::hash() const {
  if (!url_.fragment || url_.fragment.value().empty()) {
    return {};
  }

  return "#" + url_.fragment.value();
}

tl::expected<void, std::error_code> url::set_hash(string_type &&hash) {
  if (hash.empty()) {
    url_.fragment = std::nullopt;
    update_record(std::move(url_));
    return {};
  }

  auto input = hash;
  if (input.front() == '#') {
    auto first = std::begin(input), last = std::end(input);
    input.assign(first + 1, last);
  }

  url_.fragment = "";
  return details::basic_parse(std::move(input), std::nullopt, url_,
                              url_parse_state::fragment)
      .and_then([this](auto &&new_url) -> tl::expected<void, std::error_code> {
        update_record(std::forward<url_record>(new_url));
        return {};
      });
}

std::optional<std::uint16_t> url::default_port(const url::string_type &scheme) noexcept {
  return details::default_port(string_view(scheme));
}

void url::clear() {
  update_record(url_record{});
}

void swap(url &lhs, url &rhs) noexcept {
  lhs.swap(rhs);
}

namespace details {
tl::expected<url, std::error_code> make_url(
    url::string_type &&input,
    const std::optional<url_record> &base) {
  return parse(input, base)
      .and_then([](auto &&new_url) -> tl::expected<url, std::error_code> {
        return url(std::forward<url_record>(new_url));
      });
}
}  // namespace details
}  // namespace skyr
