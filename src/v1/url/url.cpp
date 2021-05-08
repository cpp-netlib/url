// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/v1/url.hpp>
#include <skyr/v1/core/parse.hpp>
#include <skyr/v1/core/serialize.hpp>
#include <skyr/v1/percent_encoding/percent_encoded_char.hpp>
#include <skyr/v1/domain/domain.hpp>
#include "v1/core/url_parse_impl.hpp"

namespace skyr {
inline namespace v1 {
void url::initialize(string_view input, const url_record *base) {
  using result_type = tl::expected<void, std::error_code>;

  bool validation_error = false;
  details::parse(input, &validation_error, base)
      .and_then([this](auto &&url) -> result_type {
        update_record(std::forward<url_record>(url));
        return {};
      })
      .or_else([](auto &&error) -> result_type {
        SKYR_EXCEPTIONS_THROW(url_parse_error(error));
        return {};
      });
}

void url::initialize(string_view input) {
  initialize(input, nullptr);
}

auto url::set_href(string_view href) -> std::error_code {
  bool validation_error = false;
  auto new_url = details::basic_parse(href, &validation_error, nullptr, nullptr, std::nullopt);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::origin() const -> string_type {
  if (url_.scheme == "blob") {
    auto url = details::make_url(pathname(), nullptr);
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

auto url::set_protocol(string_view protocol) -> std::error_code {
  auto protocol_ = static_cast<string_type>(protocol);
  if (protocol_.back() != ':') {
    protocol_ += ':';
    protocol = string_view(protocol_);
  }

  bool validation_error = false;
  auto new_url = details::basic_parse(
      protocol, &validation_error, nullptr, &url_, url_parse_state::scheme_start);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::set_username(string_view username) -> std::error_code {
  if (url_.cannot_have_a_username_password_or_port()) {
    return make_error_code(
        url_parse_errc::cannot_have_a_username_password_or_port);
  }

  auto new_url = url_;

  new_url.username.clear();
  for (auto c : username) {
    auto pct_encoded = percent_encode_byte(std::byte(c), percent_encoding::encode_set::userinfo);
    new_url.username += pct_encoded.to_string();
  }

  update_record(std::move(new_url));
  return {};
}

auto url::set_password(string_view password) -> std::error_code {
  if (url_.cannot_have_a_username_password_or_port()) {
    return make_error_code(
        url_parse_errc::cannot_have_a_username_password_or_port);
  }

  auto new_url = url_;

  new_url.password.clear();
  for (auto c : password) {
    auto pct_encoded = percent_encode_byte(std::byte(c), percent_encoding::encode_set::userinfo);
    new_url.password += pct_encoded.to_string();
  }

  update_record(std::move(new_url));
  return {};
}

auto url::set_host(string_view host) -> std::error_code {
  if (url_.cannot_be_a_base_url) {
    return make_error_code(
        url_parse_errc::cannot_be_a_base_url);
  }

  bool validation_error = false;
  auto new_url = details::basic_parse(
      host, &validation_error, nullptr, &url_, url_parse_state::host);
  if (!new_url) {
    if (new_url.error() == url_parse_errc::invalid_port) {
      new_url = details::basic_parse(
          host, &validation_error, nullptr, &url_, url_parse_state::hostname);
      if (!new_url) {
        return new_url.error();
      }
    }
    else {
      return new_url.error();
    }
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::set_hostname(string_view hostname) -> std::error_code {
  if (url_.cannot_be_a_base_url) {
    return make_error_code(
        url_parse_errc::cannot_be_a_base_url);
  }

  bool validation_error = false;
  auto new_url = details::basic_parse(
      hostname, &validation_error, nullptr, &url_, url_parse_state::hostname);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::is_ipv4_address() const -> bool {
  return (url_.host && url_.host.value().is_ipv4_address());
}

auto url::ipv4_address() const -> std::optional<skyr::ipv4_address> {
  if (!is_ipv4_address()) {
    return std::nullopt;
  }
  return url_.host.value().ipv4_address();
}

auto url::is_ipv6_address() const -> bool {
  return (url_.host && url_.host.value().is_ipv6_address());
}

auto url::ipv6_address() const -> std::optional<skyr::ipv6_address> {
  if (!is_ipv6_address()) {
    return std::nullopt;
  }
  return url_.host.value().ipv6_address();
}

auto url::domain() const -> std::optional<std::string> {
  return url_.host? url_.host.value().domain_name() : std::nullopt;
}

auto url::u8domain() const -> std::optional<std::string> {
  auto domain = this->domain();
  if (domain) {
    auto u8_domain = std::string{};
    return domain_to_u8(domain.value(), &u8_domain) ? std::make_optional(u8_domain) : std::nullopt;
  }
  return domain;
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
    bool validation_error = false;
    auto new_url = details::basic_parse(
        port, &validation_error, nullptr, &url_, url_parse_state::port);
    if (!new_url) {
      return new_url.error();
    }
    update_record(std::move(new_url).value());
  }

  return {};
}

auto  url::set_pathname(string_view pathname) -> std::error_code {
  if (url_.cannot_be_a_base_url) {
    return make_error_code(
        url_parse_errc::cannot_be_a_base_url);
  }

  url_.path.clear();
  bool validation_error = false;
  auto new_url = details::basic_parse(
      pathname, &validation_error, nullptr, &url_, url_parse_state::path_start);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
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
  bool validation_error = false;
  auto new_url = details::basic_parse(
      search, &validation_error, nullptr, &url, url_parse_state::query);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
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
  bool validation_error = false;
  auto new_url = details::basic_parse(hash, &validation_error, nullptr, &url_, url_parse_state::fragment);
  if (!new_url) {
    return new_url.error();
  }
  update_record(std::move(new_url).value());
  return {};
}

auto url::default_port(std::string_view scheme) noexcept -> std::optional<std::uint16_t> {
  return skyr::default_port(scheme);
}

namespace details {
auto make_url(
    std::string_view input,
    const url_record *base) -> tl::expected<url, url_parse_errc> {
  bool validation_error = false;
  return parse(input, &validation_error, base)
      .and_then([](auto &&new_url) -> tl::expected<url, url_parse_errc> {
        return url(std::forward<url_record>(new_url));
      });
}
}  // namespace details

void url::update_record(url_record &&url) {
  url_ = url;
  href_ = serialize(url_);
  view_ = string_view(href_);
  parameters_.initialize(
      url_.query ? string_view(url_.query.value()) : string_view{});
}
}  // namespace v1
}  // namespace skyr
