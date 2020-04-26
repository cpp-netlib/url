// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <limits>
#include <array>
#include <skyr/network/ipv4_address.hpp>
#include <skyr/network/ipv6_address.hpp>
#include <skyr/domain/domain.hpp>
#include <skyr/core/schemes.hpp>
#include <skyr/percent_encoding/percent_decode_range.hpp>
#include "url_parser_context.hpp"
#include "string/starts_with.hpp"
#include "string/locale.hpp"

namespace skyr {
inline namespace v1 {
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace {
inline auto contains(
    char byte,
    std::string_view view) noexcept {
  auto first = begin(view), last = end(view);
  return last != std::find(first, last, byte);
}

constexpr static auto is_not_c0_control_or_whitespace = [] (auto byte) {
  return !is_c0_control_or_whitespace(byte);
};

constexpr static auto is_tab_or_whitespace = [] (auto byte) {
  return contains(byte, "\t\r\n"sv);
};

auto remove_leading_whitespace(std::string &input) {
  auto first = begin(input), last = end(input);
  auto it = std::find_if(first, last, is_not_c0_control_or_whitespace);
  if (it != first) {
    input.assign(it, last);
  }
  return it == first;
}

auto remove_trailing_whitespace(std::string &input) {
  auto first = rbegin(input), last = rend(input);
  auto it = std::find_if(first, last, is_not_c0_control_or_whitespace);
  if (it != first) {
    input = std::string(it, last);
    std::reverse(begin(input), end(input));
  }
  return it == first;
}

auto remove_tabs_and_newlines(std::string &input) {
  auto first = begin(input), last = end(input);
  auto it = std::remove_if(first, last, is_tab_or_whitespace);
  input.erase(it, end(input));
  return it == last;
}

inline auto is_forbidden_host_point(std::string_view::value_type byte) noexcept {
  using namespace std::string_view_literals;
  constexpr auto forbidden = "\0\t\n\r #%/:?@[\\]"sv;
  auto first = begin(forbidden), last = end(forbidden);
  return last != std::find(first, last, byte);
}

auto remaining_starts_with(
    std::string_view input,
    std::string_view chars) noexcept {
  if (input.empty()) {
    return false;
  }
  return starts_with(input.substr(1), chars);
}

auto parse_opaque_host(std::string_view input) -> tl::expected<std::string, url_parse_errc> {
  auto first = begin(input), last = end(input);
  auto it = std::find_if(
      first, last, [] (auto byte) -> bool {
        return (byte != '%') && is_forbidden_host_point(byte);
      });
  if (it != last) {
      // result.validation_error = true;
      return tl::make_unexpected(url_parse_errc::forbidden_host_point);
    }

  auto output = std::string();
  for (auto c : input) {
    auto pct_encoded = percent_encode_byte(c, percent_encoding::encode_set::c0_control);
    output += pct_encoded.to_string();
  }
  return output;
}

auto parse_host(
    std::string_view input, bool is_not_special = false) -> tl::expected<std::string, url_parse_errc> {
  if (!input.empty() && (input.front() == '[')) {
    if (input.back() != ']') {
      // result.validation_error = true;
      return tl::make_unexpected(url_parse_errc::invalid_ipv6_address);
    }

    auto view = std::string_view(input);
    view.remove_prefix(1);
    view.remove_suffix(1);
    auto ipv6_address = parse_ipv6_address(view);
    if (ipv6_address) {
      return "[" + ipv6_address.value().serialize() + "]";
    }
    else {
      return tl::make_unexpected(url_parse_errc::invalid_ipv6_address);
    }
  }

  if (is_not_special) {
    return parse_opaque_host(input);
  }

  auto domain = percent_encoding::as<std::string>(
      input | percent_encoding::view::decode);
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
    // result.validation_error = true;
    return tl::make_unexpected(url_parse_errc::domain_error);
  }

  auto host = parse_ipv4_address(ascii_domain.value());
  if (!host) {
    if (host.error() == make_error_code(ipv4_address_errc::overflow)) {
      return tl::make_unexpected(url_parse_errc::invalid_ipv4_address);
    }
    else {
      return ascii_domain.value();
    }
  }
  return host.value().serialize();
}

auto port_number(std::string_view port) noexcept -> tl::expected<std::uint16_t, url_parse_errc> {
  if (port.empty()) {
    return tl::make_unexpected(url_parse_errc::invalid_port);
  }

  const char* port_first = port.data();
  char* port_last = nullptr;
  auto port_value = std::strtoul(port_first, &port_last, 10);

  if (port_first == port_last) {
    return tl::make_unexpected(url_parse_errc::invalid_port);
  }

  if (port_value > std::numeric_limits<std::uint16_t>::max()) {
    return tl::make_unexpected(url_parse_errc::invalid_port);
  }
  return static_cast<std::uint16_t>(port_value);
}

auto is_url_code_point(char byte) noexcept {
  return
      std::isalnum(byte, std::locale::classic()) || contains(byte, "!$&'()*+,-./:;=?@_~"sv);
}

inline auto is_windows_drive_letter(std::string_view segment) noexcept {
  if (segment.size() < 2) {
    return false;
  }

  if (!std::isalpha(segment[0], std::locale::classic())) {
    return false;
  }

  auto result = ((segment[1] == ':') || (segment[1] == '|'));
  if (result) {
    if (segment.size() > 2) {
      result = ((segment[2] == '/') || (segment[2] == '\\') || (segment[2] == '?') || (segment[2] == '#'));
    }
  }
  return result;
}

inline auto is_single_dot_path_segment(std::string_view segment) noexcept {
  auto lower = std::string(segment);
  std::transform(begin(lower), end(lower), begin(lower),
                 [] (auto byte) -> decltype(byte) {
    return std::tolower(byte, std::locale::classic());
  });

  return ((lower == ".") || (lower == "%2e"));
}

auto is_double_dot_path_segment(std::string_view segment) noexcept {
  auto lower = std::string(segment);
  std::transform(begin(lower), end(lower), begin(lower),
                 [] (auto byte) -> decltype(byte) {
    return std::tolower(byte, std::locale::classic());
  });

  return (
      (lower == "..") ||
      (lower == ".%2e") ||
      (lower == "%2e.") ||
      (lower == "%2e%2e"));
}

void shorten_path(std::string_view scheme, std::vector<std::string> &path) {
  if (path.empty()) {
    return;
  }

  if ((scheme == "file"sv) &&
      (path.size() == 1) &&
      is_windows_drive_letter(path.front())) {
    return;
  }

  path.pop_back();
}
} // namespace

url_parser_context::url_parser_context(
    std::string_view input,
    const url_record *base,
    const url_record *url,
    std::optional<url_parse_state> state_override)
    : input(input)
    , base(base)
    , url(url? *url : url_record{})
    , state(state_override? state_override.value() : url_parse_state::scheme_start)
    , state_override(state_override)
    , buffer()
    , at_flag(false)
    , square_braces_flag(false)
    , password_token_seen_flag(false) {
  this->url.validation_error |= !remove_leading_whitespace(this->input);
  this->url.validation_error |= !remove_trailing_whitespace(this->input);
  this->url.validation_error |= !remove_tabs_and_newlines(this->input);

  view = std::string_view(this->input);
  it = begin(view);
}

auto url_parser_context::parse_scheme_start(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (std::isalpha(byte, std::locale::classic())) {
    auto lower = std::tolower(byte, std::locale::classic());
    buffer.push_back(lower);
    state = url_parse_state::scheme;
  } else if (!state_override) {
    state = url_parse_state::no_scheme;
    reset();
    return url_parse_action::continue_;
  } else {
    url.validation_error = true;
    return tl::make_unexpected(url_parse_errc::invalid_scheme_character);
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_scheme(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (std::isalnum(byte, std::locale::classic()) || contains(byte, "+-."sv)) {
    auto lower = std::tolower(byte, std::locale::classic());
    buffer.push_back(lower);
  } else if (byte == ':') {
    if (state_override) {
      if (url.is_special() && !is_special(buffer)) {
        return tl::make_unexpected(url_parse_errc::cannot_override_scheme);
      }

      if (!url.is_special() && is_special(buffer)) {
        return tl::make_unexpected(url_parse_errc::cannot_override_scheme);
      }

      if ((url.includes_credentials() || url.port) &&(buffer == "file")) {
        return tl::make_unexpected(url_parse_errc::cannot_override_scheme);
      }

      if ((url.scheme == "file") && (!url.host || url.host.value().empty())) {
        return tl::make_unexpected(url_parse_errc::cannot_override_scheme);
      }
    }

    url.scheme = buffer;

    if (state_override) {
      if (url.port == default_port(url.scheme)) {
        url.port = std::nullopt;
      }
      return url_parse_action::success;
    }
    buffer.clear();

    if (url.scheme == "file") {
      if (!remaining_starts_with(view.substr(std::distance(std::begin(view), it)), "//"sv)) {
        url.validation_error = true;
      }
      state = url_parse_state::file;
    } else if (url.is_special() && base && (base->scheme == url.scheme)) {
      state = url_parse_state::special_relative_or_authority;
    } else if (url.is_special()) {
      state = url_parse_state::special_authority_slashes;
    } else if (remaining_starts_with(view.substr(std::distance(std::begin(view), it)), "/"sv)) {
      state = url_parse_state::path_or_authority;
      increment();
    } else {
      url.cannot_be_a_base_url = true;
      url.path.emplace_back();
      state = url_parse_state::cannot_be_a_base_url_path;
    }
  } else if (!state_override) {
    buffer.clear();
    state = url_parse_state::no_scheme;
    reset();
    return url_parse_action::continue_;
  }
  else {
    return tl::make_unexpected(url_parse_errc::invalid_scheme_character);
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_no_scheme(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (!base || (base->cannot_be_a_base_url && (byte != '#'))) {
    url.validation_error = true;
    return tl::make_unexpected(url_parse_errc::not_an_absolute_url_with_fragment);
  } else if (base->cannot_be_a_base_url && (byte == '#')) {
    url.scheme = base->scheme;
    url.path = base->path;
    url.query = base->query;
    url.fragment = std::string();

    url.cannot_be_a_base_url = true;
    state = url_parse_state::fragment;
  } else if (base->scheme != "file") {
    state = url_parse_state::relative;
    reset();
    return url_parse_action::continue_;
  } else {
    state = url_parse_state::file;
    reset();
    return url_parse_action::continue_;
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_special_relative_or_authority(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if ((byte == '/') && remaining_starts_with(view.substr(std::distance(std::begin(view), it)), "/"sv)) {
    increment();
    state = url_parse_state::special_authority_ignore_slashes;
  } else {
    url.validation_error = true;
    decrement();
    state = url_parse_state::relative;
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_path_or_authority(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (byte == '/') {
    state = url_parse_state::authority;
  } else {
    state = url_parse_state::path;
    decrement();
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_relative(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  url.scheme = base->scheme;
  if (is_eof()) {
    url.username = base->username;
    url.password = base->password;
    url.host = base->host;
    url.port = base->port;
    url.path = base->path;
    url.query = base->query;
  }
  else if (byte == '/') {
    state = url_parse_state::relative_slash;
  } else if (byte == '?') {
    url.username = base->username;
    url.password = base->password;
    url.host = base->host;
    url.port = base->port;
    url.path = base->path;
    url.query = std::string();
    state = url_parse_state::query;
  } else if (byte == '#') {
    url.username = base->username;
    url.password = base->password;
    url.host = base->host;
    url.port = base->port;
    url.path = base->path;
    url.query = base->query;
    url.fragment = std::string();
    state = url_parse_state::fragment;
  } else {
    if (url.is_special() && (byte == '\\')) {
      url.validation_error = true;
      state = url_parse_state::relative_slash;
    } else {
      url.username = base->username;
      url.password = base->password;
      url.host = base->host;
      url.port = base->port;
      url.path = base->path;
      if (!url.path.empty()) {
        url.path.pop_back();
      }
      state = url_parse_state::path;

      if (it == begin(view)) {
        return url_parse_action::continue_;
      }

      decrement();
    }
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_relative_slash(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (url.is_special() && ((byte == '/') || (byte == '\\'))) {
    if (byte == '\\') {
      url.validation_error = true;
    }
    state = url_parse_state::special_authority_ignore_slashes;
  }
  else if (byte == '/') {
    state = url_parse_state::authority;
  }
  else {
    url.username = base->username;
    url.password = base->password;
    url.host = base->host;
    url.port = base->port;
    state = url_parse_state::path;
    decrement();
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_special_authority_slashes(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if ((byte == '/') && remaining_starts_with(view.substr(std::distance(std::begin(view), it)), "/"sv)) {
    increment();
    state = url_parse_state::special_authority_ignore_slashes;
  } else {
    url.validation_error = true;
    decrement();
    state = url_parse_state::special_authority_ignore_slashes;
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_special_authority_ignore_slashes(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if ((byte != '/') && (byte != '\\')) {
    decrement();
    state = url_parse_state::authority;
  } else {
    url.validation_error = true;
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_authority(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (byte == '@') {
    url.validation_error = true;
    if (at_flag) {
      buffer = "%40" + buffer;
    }
    at_flag = true;

    for (auto c : buffer) {
      if (c == ':' && !password_token_seen_flag) {
        password_token_seen_flag = true;
        continue;
      }

      auto pct_encoded = percent_encode_byte(c, percent_encoding::encode_set::userinfo);
      if (password_token_seen_flag) {
        url.password += pct_encoded.to_string();
      } else {
        url.username += pct_encoded.to_string();
      }
    }
    buffer.clear();
  } else if (
      ((is_eof()) || (byte == '/') || (byte == '?') || (byte == '#')) ||
          (url.is_special() && (byte == '\\'))) {
    if (at_flag && buffer.empty()) {
      url.validation_error = true;
      return tl::make_unexpected(url_parse_errc::empty_hostname);
    }
    restart_from_buffer();
    state = url_parse_state::host;
    buffer.clear();
    return url_parse_action::increment;
  } else {
    buffer.push_back(byte);
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_hostname(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (state_override && (url.scheme == "file")) {
    state = url_parse_state::file_host;
    if (it == begin(view)) {
      return url_parse_action::continue_;
    }
    decrement();
  } else if ((byte == ':') && !square_braces_flag) {
    if (buffer.empty()) {
      url.validation_error = true;
      return tl::make_unexpected(url_parse_errc::empty_hostname);
    }

    auto host = parse_host(buffer, !url.is_special());
    if (!host) {
      return tl::make_unexpected(host.error());
    }
    url.host = host.value();
    buffer.clear();
    state = url_parse_state::port;

    if (state_override && (state_override.value() == url_parse_state::hostname)) {
      return url_parse_action::success;
    }
  } else if (
      (is_eof() || (byte == '/') || (byte == '?') || (byte == '#')) ||
          (url.is_special() && (byte == '\\'))) {
    if (it != begin(view)) {
      decrement();
    }

    if (url.is_special() && buffer.empty()) {
      url.validation_error = true;
      return tl::make_unexpected(url_parse_errc::empty_hostname);
    }
    else if (
        state_override &&
        buffer.empty() &&
        (url.includes_credentials() || url.port)) {
      url.validation_error = true;
      return url_parse_action::success;
    }

    auto host = parse_host(buffer, !url.is_special());
    if (!host) {
      return tl::make_unexpected(host.error());
    }
    url.host = host.value();
    buffer.clear();
    state = url_parse_state::path_start;

    if (state_override) {
      return url_parse_action::success;
    }
  } else {
    if (byte == '[') {
      square_braces_flag = true;
    } else if (byte == ']') {
      square_braces_flag = false;
    }
    buffer += byte;
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_port(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (std::isdigit(byte, std::locale::classic())) {
    buffer += byte;
  } else if (
      ((is_eof()) || (byte == '/') || (byte == '?') || (byte == '#')) ||
          (url.is_special() && (byte == '\\')) ||
          state_override) {
    if (!buffer.empty()) {
      auto port = port_number(buffer);

      if (!port) {
        url.validation_error = true;
        return tl::make_unexpected(port.error());
      }

      auto dport = default_port(url.scheme);
      if (dport && (dport.value() == port.value())) {
        url.port = std::nullopt;
      }
      else {
        url.port = port.value();
      }
      buffer.clear();
    }

    if (state_override) {
      return url_parse_action::success;
    }

    decrement();
    state = url_parse_state::path_start;
  } else {
    url.validation_error = true;
    return tl::make_unexpected(url_parse_errc::invalid_port);
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_file(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  url.scheme = "file";

  if ((byte == '/') || (byte == '\\')) {
    if (byte == '\\') {
      url.validation_error = true;
    }
    state = url_parse_state::file_slash;
  } else if (base && (base->scheme == "file")) {
    if (is_eof()) {
      url.host = base->host;
      url.path = base->path;
      url.query = base->query;
    }
    else if (byte == '?') {
      url.host = base->host;
      url.path = base->path;
      url.query = std::string();
      state = url_parse_state::query;
    } else if (byte == '#') {
      url.host = base->host;
      url.path = base->path;
      url.query = base->query;
      url.fragment = std::string();
      state = url_parse_state::fragment;
    } else {
      if (!is_windows_drive_letter(view.substr(std::distance(begin(view), it)))) {
        url.host = base->host;
        url.path = base->path;
        shorten_path(url.scheme, url.path);
      }
      else {
        url.validation_error = true;
      }
      state = url_parse_state::path;
      if (it == begin(view)) {
        return url_parse_action::continue_;
      }
      decrement();
    }
  } else {
    state = url_parse_state::path;
    if (it == begin(view)) {
      return url_parse_action::continue_;
    }
    decrement();
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_file_slash(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if ((byte == '/') || (byte == '\\')) {
    if (byte == '\\') {
      url.validation_error = true;
    }
    state = url_parse_state::file_host;
  } else {
    if (base &&
            ((base->scheme == "file") && !is_windows_drive_letter(view.substr(std::distance(begin(view), it))))) {
      if (!base->path.empty() && is_windows_drive_letter(base->path[0])) {
        url.path.push_back(base->path[0]);
      } else {
        url.host = base->host;
      }
    }

    state = url_parse_state::path;
    decrement();
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_file_host(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if ((is_eof()) || (byte == '/') || (byte == '\\') || (byte == '?') || (byte == '#')) {
    bool at_begin = (it == begin(view));
    if (!at_begin) {
      decrement();
    }

    if (!state_override && is_windows_drive_letter(buffer)) {
      url.validation_error = true;
      state = url_parse_state::path;
    } else if (buffer.empty()) {
      url.host = std::string();

      if (state_override) {
        return url_parse_action::success;
      }

      state = url_parse_state::path_start;
    } else {
      auto host = parse_host(buffer, !url.is_special());
      if (!host) {
        return tl::make_unexpected(host.error());
      }

      if (host.value() == "localhost") {
        host.value().clear();
      }
      url.host = host.value();

      if (state_override) {
        return url_parse_action::success;
      }

      buffer.clear();

      state = url_parse_state::path_start;
    }
  } else {
    buffer.push_back(byte);
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_path_start(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  bool at_begin = (it == begin(view));
  if (url.is_special()) {
    if (byte == '\\') {
      url.validation_error = true;
    }
    state = url_parse_state::path;
    if ((byte != '/') && (byte != '\\')) {
      if (at_begin) {
        return url_parse_action::continue_;
      }
      decrement();
    }
  } else if (!state_override && (byte == '?')) {
    url.query = std::string();
    state = url_parse_state::query;
  } else if (!state_override && (byte == '#')) {
    url.fragment = std::string();
    state = url_parse_state::fragment;
  } else if (!is_eof()) {
    state = url_parse_state::path;
    if (byte != '/') {
      if (at_begin) {
        return url_parse_action::continue_;
      }
      decrement();
    }
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_path(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (((is_eof()) || (byte == '/')) ||
      (url.is_special() && (byte == '\\')) ||
      (!state_override && ((byte == '?') || (byte == '#')))) {
    if (url.is_special() && (byte == '\\')) {
      url.validation_error = true;
    }

    if (is_double_dot_path_segment(buffer)) {
      shorten_path(url.scheme, url.path);
      if (!((byte == '/') || (url.is_special() && (byte == '\\')))) {
        url.path.emplace_back();
      }
    } else if (
        is_single_dot_path_segment(buffer) &&
            !((byte == '/') || (url.is_special() && (byte == '\\')))) {
      url.path.emplace_back();
    } else if (!is_single_dot_path_segment(buffer)) {
      if ((url.scheme == "file") &&
          url.path.empty() && is_windows_drive_letter(buffer)) {
        if (!url.host || !url.host.value().empty()) {
          url.validation_error = true;
          url.host = std::string();
        }
        buffer[1] = ':';
      }

      url.path.push_back(buffer);
    }

    buffer.clear();

    if ((url.scheme == "file") && (is_eof() || (byte == '?') || (byte == '#'))) {
      while ((url.path.size() > 1) && url.path[0].empty()) {
        url.validation_error = true;
        auto next_it = begin(url.path);
        ++next_it;
        std::rotate(begin(url.path), next_it, end(url.path));
        url.path.pop_back();
      }
    }

    if (byte == '?') {
      url.query = std::string();
      state = url_parse_state::query;
    }

    if (byte == '#') {
      url.fragment = std::string();
      state = url_parse_state::fragment;
    }
  } else {
    if (!is_url_code_point(byte) && (byte != '%')) {
      url.validation_error = true;
    }

    auto pct_encoded = percent_encode_byte(byte, percent_encoding::encode_set::path);
    buffer += pct_encoded.to_string();
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_cannot_be_a_base_url(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (byte == '?') {
    url.query = std::string();
    state = url_parse_state::query;
  } else if (byte == '#') {
    url.fragment = std::string();
    state = url_parse_state::fragment;
  } else {
    if (!is_eof() && (!is_url_code_point(byte) && (byte != '%'))) {
      url.validation_error = true;
    }
    else if ((byte == '%') && !percent_encoding::is_percent_encoded(
                                  view.substr(std::distance(std::begin(view), it)))) {
      url.validation_error = true;
    }
    if (!is_eof()) {
      auto pct_encoded = percent_encode_byte(byte, percent_encoding::encode_set::c0_control);
      url.path[0] += pct_encoded.to_string();
    }
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_query(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (!state_override && (byte == '#')) {
    url.fragment = std::string();
    state = url_parse_state::fragment;
  } else if (!is_eof()) {
    if ((byte < '!') ||
        (byte > '~') ||
        (contains(byte, R"("#<>)"sv)) ||
        ((byte == '\'') && url.is_special())) {
      auto pct_encoded = percent_encode_byte(byte, percent_encoding::encode_set::query);
      url.query.value() += pct_encoded.to_string();
    } else {
      url.query.value().push_back(byte);
    }
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_fragment(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (byte == '\0') {
    url.validation_error = true;
  } else {
    auto pct_encoded = percent_encode_byte(byte, percent_encoding::encode_set::fragment);
    url.fragment.value() += pct_encoded.to_string();
  }
  return url_parse_action::increment;
}
}  // namespace v1
}  // namespace skyr
