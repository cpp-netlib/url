// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <limits>
#include <array>
#include <locale>
#include <skyr/v1/domain/domain.hpp>
#include <skyr/v1/core/schemes.hpp>
#include <skyr/v1/core/host.hpp>
#include <skyr/v1/percent_encoding/percent_encoded_char.hpp>
#include <skyr/v1/string/starts_with.hpp>
#include "url_parser_context.hpp"

namespace skyr {
inline namespace v1 {
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace {
auto contains(std::string_view view, char byte) noexcept {
  auto first = std::cbegin(view), last = std::cend(view);
  return last != std::find(first, last, byte);
}

auto remaining_starts_with(std::string_view input, std::string_view chars) noexcept {
  return !input.empty() && starts_with(input.substr(1), chars);
}

auto port_number(std::string_view port) noexcept -> tl::expected<std::uint16_t, url_parse_errc> {
  if (port.empty()) {
    return tl::make_unexpected(url_parse_errc::invalid_port);
  }

  const char *port_first = port.data();
  char *port_last = nullptr;
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
  return std::isalnum(byte, std::locale::classic()) || contains("!$&'()*+,-./:;=?@_~"sv, byte);
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
  return (segment == ".") || (segment == "%2e") || (segment == "%2E");
}

auto is_double_dot_path_segment(std::string_view segment) noexcept {
  return (segment == "..") || (segment == "%2e.") || (segment == ".%2e") || (segment == "%2e%2e") ||
         (segment == "%2E.") || (segment == ".%2E") || (segment == "%2E%2E") || (segment == "%2E%2e") ||
         (segment == "%2e%2E");
}

void shorten_path(std::string_view scheme, std::vector<std::string> &path) {
  if (!path.empty() && !((scheme == "file"sv) && (path.size() == 1) && is_windows_drive_letter(path.front()))) {
    path.pop_back();
  }
}
}  // namespace

url_parser_context::url_parser_context(std::string_view input, bool *validation_error, const url_record *base,
                                       const url_record *url, std::optional<url_parse_state> state_override)
    : input(input),
      it(begin(input)),
      validation_error(validation_error),
      base(base),
      url(url ? *url : url_record{}),
      state(state_override ? state_override.value() : url_parse_state::scheme_start),
      state_override(state_override),
      buffer(),
      at_flag(false),
      square_braces_flag(false),
      password_token_seen_flag(false) {
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
    *validation_error |= true;
    return tl::make_unexpected(url_parse_errc::invalid_scheme_character);
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_scheme(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (std::isalnum(byte, std::locale::classic()) || contains("+-."sv, byte)) {
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

      if ((url.includes_credentials() || url.port) && (buffer == "file")) {
        return tl::make_unexpected(url_parse_errc::cannot_override_scheme);
      }

      if ((url.scheme == "file") && (!url.host || url.host.value().is_empty())) {
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

    auto substr = input.substr(std::distance(std::begin(input), it));

    if (url.scheme == "file") {
      if (!remaining_starts_with(substr, "//"sv)) {
        *validation_error |= true;
      }
      state = url_parse_state::file;
    } else if (url.is_special() && base && (base->scheme == url.scheme)) {
      state = url_parse_state::special_relative_or_authority;
    } else if (url.is_special()) {
      state = url_parse_state::special_authority_slashes;
    } else if (remaining_starts_with(substr, "/"sv)) {
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
  } else {
    return tl::make_unexpected(url_parse_errc::invalid_scheme_character);
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_no_scheme(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (!base || (base->cannot_be_a_base_url && (byte != '#'))) {
    *validation_error |= true;
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

auto url_parser_context::parse_special_relative_or_authority(char byte)
    -> tl::expected<url_parse_action, url_parse_errc> {
  auto substr = input.substr(std::distance(std::begin(input), it));
  if ((byte == '/') && remaining_starts_with(substr, "/"sv)) {
    increment();
    state = url_parse_state::special_authority_ignore_slashes;
  } else {
    *validation_error |= true;
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
  } else if (byte == '/') {
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
      *validation_error |= true;
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

      if (it == begin(input)) {
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
      *validation_error |= true;
    }
    state = url_parse_state::special_authority_ignore_slashes;
  } else if (byte == '/') {
    state = url_parse_state::authority;
  } else {
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
  auto substr = input.substr(std::distance(std::begin(input), it));
  if ((byte == '/') && remaining_starts_with(substr, "/"sv)) {
    increment();
    state = url_parse_state::special_authority_ignore_slashes;
  } else {
    *validation_error |= true;
    decrement();
    state = url_parse_state::special_authority_ignore_slashes;
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_special_authority_ignore_slashes(char byte)
    -> tl::expected<url_parse_action, url_parse_errc> {
  if ((byte != '/') && (byte != '\\')) {
    decrement();
    state = url_parse_state::authority;
  } else {
    *validation_error |= true;
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_authority(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (byte == '@') {
    *validation_error |= true;
    if (at_flag) {
      buffer.insert(0, "%40");
    }
    at_flag = true;

    for (auto c : buffer) {
      if (c == ':' && !password_token_seen_flag) {
        password_token_seen_flag = true;
        continue;
      }

      auto pct_encoded = percent_encode_byte(std::byte(c), percent_encoding::encode_set::userinfo);
      if (password_token_seen_flag) {
        url.password += pct_encoded.to_string();
      } else {
        url.username += pct_encoded.to_string();
      }
    }
    buffer.clear();
  } else if (((is_eof()) || (byte == '/') || (byte == '?') || (byte == '#')) || (url.is_special() && (byte == '\\'))) {
    if (at_flag && buffer.empty()) {
      *validation_error |= true;
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
    if (it == begin(input)) {
      return url_parse_action::continue_;
    }
    decrement();
  } else if ((byte == ':') && !square_braces_flag) {
    if (buffer.empty()) {
      *validation_error |= true;
      return tl::make_unexpected(url_parse_errc::empty_hostname);
    }

    auto host = parse_host(buffer, !url.is_special(), validation_error);
    if (!host) {
      return tl::make_unexpected(host.error());
    }
    url.host = host.value();
    buffer.clear();
    state = url_parse_state::port;

    if (state_override && (state_override.value() == url_parse_state::hostname)) {
      return url_parse_action::success;
    }
  } else if ((is_eof() || (byte == '/') || (byte == '?') || (byte == '#')) || (url.is_special() && (byte == '\\'))) {
    if (it != begin(input)) {
      decrement();
    }

    if (url.is_special() && buffer.empty()) {
      *validation_error |= true;
      return tl::make_unexpected(url_parse_errc::empty_hostname);
    } else if (state_override && buffer.empty() && (url.includes_credentials() || url.port)) {
      *validation_error |= true;
      return url_parse_action::success;
    }

    auto host = parse_host(buffer, !url.is_special(), validation_error);
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
  } else if (((is_eof()) || (byte == '/') || (byte == '?') || (byte == '#')) || (url.is_special() && (byte == '\\')) ||
             state_override) {
    if (!buffer.empty()) {
      auto port = port_number(buffer);

      if (!port) {
        *validation_error |= true;
        return tl::make_unexpected(port.error());
      }

      auto dport = default_port(url.scheme);
      if (dport && (dport.value() == port.value())) {
        url.port = std::nullopt;
      } else {
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
    *validation_error |= true;
    return tl::make_unexpected(url_parse_errc::invalid_port);
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_file(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  url.scheme = "file";
  url.host = host{empty_host{}};

  if ((byte == '/') || (byte == '\\')) {
    if (byte == '\\') {
      *validation_error |= true;
    }
    state = url_parse_state::file_slash;
  } else if (base && (base->scheme == "file")) {
    url.host = base->host;
    url.path = base->path;
    url.query = base->query;
    if (byte == '?') {
      url.query = std::string();
      state = url_parse_state::query;
    } else if (byte == '#') {
      url.fragment = std::string();
      state = url_parse_state::fragment;
    } else {
      url.query = std::nullopt;
      if (!is_windows_drive_letter(input.substr(std::distance(begin(input), it)))) {
        shorten_path(url.scheme, url.path);
      } else {
        *validation_error |= true;
        url.path.clear();
      }
      state = url_parse_state::path;
      if (it == begin(input)) {
        return url_parse_action::continue_;
      }
      decrement();
    }
  } else {
    state = url_parse_state::path;
    if (it == begin(input)) {
      return url_parse_action::continue_;
    }
    decrement();
  }

  return url_parse_action::increment;
}

auto url_parser_context::parse_file_slash(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if ((byte == '/') || (byte == '\\')) {
    if (byte == '\\') {
      *validation_error |= true;
    }
    state = url_parse_state::file_host;
  } else {
    if (base && (base->scheme == "file")) {
      auto substr = input.substr(std::distance(begin(input), it));
      if (!is_windows_drive_letter(substr) &&
          (!base->path.empty() && is_windows_drive_letter(base->path[0]))) {
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
    bool at_begin = (it == begin(input));
    if (!at_begin) {
      decrement();
    }

    if (!state_override && is_windows_drive_letter(buffer)) {
      *validation_error |= true;
      state = url_parse_state::path;
    } else if (buffer.empty()) {
      url.host = skyr::host{skyr::v1::empty_host{}};

      if (state_override) {
        return url_parse_action::success;
      }

      state = url_parse_state::path_start;
    } else {
      auto host = parse_host(buffer, !url.is_special(), validation_error);
      if (!host) {
        return tl::make_unexpected(host.error());
      }

      if (host.value().serialize() == "localhost") {
        host.value() = skyr::host{skyr::v1::empty_host{}};
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
  bool at_begin = (it == begin(input));
  if (url.is_special()) {
    if (byte == '\\') {
      *validation_error |= true;
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
  if (((is_eof()) || (byte == '/')) || (url.is_special() && (byte == '\\')) ||
      (!state_override && ((byte == '?') || (byte == '#')))) {
    if (url.is_special() && (byte == '\\')) {
      *validation_error |= true;
    }

    if (is_double_dot_path_segment(buffer)) {
      shorten_path(url.scheme, url.path);
      if (!((byte == '/') || (url.is_special() && (byte == '\\')))) {
        url.path.emplace_back();
      }
    } else if (is_single_dot_path_segment(buffer) && !((byte == '/') || (url.is_special() && (byte == '\\')))) {
      url.path.emplace_back();
    } else if (!is_single_dot_path_segment(buffer)) {
      if ((url.scheme == "file") && url.path.empty() && is_windows_drive_letter(buffer)) {
        if (!url.host || !url.host.value().is_empty()) {
          *validation_error |= true;
          url.host = skyr::host{skyr::v1::empty_host{}};
        }
        buffer[1] = ':';
      }

      url.path.emplace_back(std::cbegin(buffer), std::cend(buffer));
    }

    buffer.clear();

    if ((url.scheme == "file") && (is_eof() || (byte == '?') || (byte == '#'))) {
      while ((url.path.size() > 1) && url.path[0].empty()) {
        *validation_error |= true;
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
      *validation_error |= true;
    }

    auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::path);
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
    auto substr = input.substr(std::distance(std::begin(input), it));
    if (!is_eof() && (!is_url_code_point(byte) && (byte != '%'))) {
      *validation_error |= true;
    } else if ((byte == '%') && !percent_encoding::is_percent_encoded(substr)) {
      *validation_error |= true;
    }
    if (!is_eof()) {
      auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::c0_control);
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
    if ((byte < '!') || (byte > '~') || (contains(R"("#<>)"sv, byte)) || ((byte == '\'') && url.is_special())) {
      auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::any);
      url.query.value() += pct_encoded.to_string();
    } else {
      url.query.value().push_back(byte);
    }
  }
  return url_parse_action::increment;
}

auto url_parser_context::parse_fragment(char byte) -> tl::expected<url_parse_action, url_parse_errc> {
  if (!is_eof()) {
    if (!is_url_code_point(byte) && (byte != '%')) {
      *validation_error |= true;
    }

    auto substr = input.substr(std::distance(std::begin(input), it));
    if ((byte == '%') && !percent_encoding::is_percent_encoded(substr)) {
      *validation_error |= true;
    }

    auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::fragment);
    url.fragment.value() += pct_encoded.to_string();
  }
  return url_parse_action::increment;
}
}  // namespace v1
}  // namespace skyr
