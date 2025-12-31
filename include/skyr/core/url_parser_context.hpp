// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_CORE_URL_PARSER_CONTEXT_HPP
#define SKYR_CORE_URL_PARSER_CONTEXT_HPP

#include <array>
#include <cassert>
#include <expected>
#include <iterator>
#include <limits>
#include <locale>
#include <optional>
#include <string_view>

#include <skyr/core/errors.hpp>
#include <skyr/core/host.hpp>
#include <skyr/core/schemes.hpp>
#include <skyr/core/url_parse_state.hpp>
#include <skyr/core/url_record.hpp>
#include <skyr/domain/domain.hpp>
#include <skyr/percent_encoding/percent_encoded_char.hpp>

namespace skyr {
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace details {
constexpr inline auto contains(std::string_view view, char element) noexcept {
  auto first = std::cbegin(view), last = std::cend(view);
  return last != std::find(first, last, element);
}

inline auto port_number(std::string_view port) noexcept -> std::expected<std::uint16_t, url_parse_errc> {
  if (port.empty()) {
    return std::unexpected(url_parse_errc::invalid_port);
  }

  const char* port_first = port.data();
  char* port_last = nullptr;  // NOLINT
  auto port_value = std::strtoul(port_first, &port_last, 10);

  if (port_first == port_last) {
    return std::unexpected(url_parse_errc::invalid_port);
  }

  if (port_value > std::numeric_limits<std::uint16_t>::max()) {
    return std::unexpected(url_parse_errc::invalid_port);
  }
  return static_cast<std::uint16_t>(port_value);
}

inline auto is_url_code_point(char byte) noexcept {
  return std::isalnum(byte, std::locale::classic()) || contains("!$&'()*+,-./:;=?@_~"sv, byte);
}

constexpr inline auto is_windows_drive_letter(std::string_view segment) noexcept {
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

constexpr inline auto is_single_dot_path_segment(std::string_view segment) noexcept {
  return (segment == ".") || (segment == "%2e") || (segment == "%2E");
}

constexpr inline auto is_double_dot_path_segment(std::string_view segment) noexcept {
  return (segment == "..") || (segment == "%2e.") || (segment == ".%2e") || (segment == "%2e%2e") ||
         (segment == "%2E.") || (segment == ".%2E") || (segment == "%2E%2E") || (segment == "%2E%2e") ||
         (segment == "%2e%2E");
}

inline void shorten_path(std::string_view scheme, std::vector<std::string>& path) {
  if (!path.empty() && !((scheme == "file"sv) && (path.size() == 1) && is_windows_drive_letter(path.front()))) {
    path.pop_back();
  }
}
}  // namespace details

enum class url_parse_action {
  success = 0,
  increment,
  continue_,
};

class url_parser_context {
 private:
  url_record url;
  url_parse_state state;
  std::string_view input;
  std::string_view::const_iterator input_it;
  bool* validation_error;
  const url_record* base;
  std::optional<url_parse_state> state_override;
  std::string buffer;

  bool at_flag;
  bool square_braces_flag;

 public:
  url_parser_context(std::string_view input, bool* validation_error, const url_record* base, const url_record* url,
                     std::optional<url_parse_state> state_override)
      : input(input)
      , input_it(begin(input))
      , validation_error(validation_error)
      , base(base)
      , url(url ? *url : url_record{})
      , state(state_override ? state_override.value() : url_parse_state::scheme_start)
      , state_override(state_override)
      , buffer()
      , at_flag(false)
      , square_braces_flag(false) {
  }

  [[nodiscard]] auto get_url() && -> url_record&& {
    return std::move(url);
  }

  [[nodiscard]] auto is_eof() const noexcept {
    return input_it == std::end(input);
  }

  [[nodiscard]] auto next_byte() const noexcept {
    return !is_eof() ? *input_it : '\0';
  }

  void increment() noexcept {
    assert(input_it != std::end(input));
    ++input_it;
  }

  auto parse_next() -> std::expected<url_parse_action, url_parse_errc> {
    auto byte = next_byte();
    switch (state) {
      case url_parse_state::scheme_start:
        return parse_scheme_start(byte);
      case url_parse_state::scheme:
        return parse_scheme(byte);
      case url_parse_state::no_scheme:
        return parse_no_scheme(byte);
      case url_parse_state::special_relative_or_authority:
        return parse_special_relative_or_authority(byte);
      case url_parse_state::path_or_authority:
        return parse_path_or_authority(byte);
      case url_parse_state::relative:
        return parse_relative(byte);
      case url_parse_state::relative_slash:
        return parse_relative_slash(byte);
      case url_parse_state::special_authority_slashes:
        return parse_special_authority_slashes(byte);
      case url_parse_state::special_authority_ignore_slashes:
        return parse_special_authority_ignore_slashes(byte);
      case url_parse_state::authority:
        return parse_authority(byte);
      case url_parse_state::hostname:
      case url_parse_state::host:
        return parse_hostname(byte);
      case url_parse_state::port:
        return parse_port(byte);
      case url_parse_state::file:
        return parse_file(byte);
      case url_parse_state::file_slash:
        return parse_file_slash(byte);
      case url_parse_state::file_host:
        return parse_file_host(byte);
      case url_parse_state::path_start:
        return parse_path_start(byte);
      case url_parse_state::path:
        return parse_path(byte);
      case url_parse_state::cannot_be_a_base_url_path:
        return parse_cannot_be_a_base_url(byte);
      case url_parse_state::query:
        return parse_query(byte);
      case url_parse_state::fragment:
        return parse_fragment(byte);
      default:
        return std::unexpected(url_parse_errc::cannot_override_scheme);
    }
  }

 private:
  void decrement() noexcept {
    assert(input_it != std::begin(input));
    --input_it;
  }

  void restart_from_beginning() noexcept {
    input_it = std::begin(input);
  }

  void restart_from_beginning_of_buffer() noexcept {
    input_it -= (buffer.size() + 1);
  }

  [[nodiscard]] auto still_to_process() const noexcept -> std::string_view {
    return input.substr(std::distance(std::begin(input), input_it));
  }

  [[nodiscard]] auto remaining_starts_with(std::string_view chars) const noexcept -> bool {
    return !still_to_process().empty() && still_to_process().substr(1).starts_with(chars);
  }

  auto parse_scheme_start(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (std::isalpha(byte, std::locale::classic())) {
      auto lower = std::tolower(byte, std::locale::classic());
      buffer.push_back(lower);
      state = url_parse_state::scheme;
    } else if (!state_override) {
      state = url_parse_state::no_scheme;
      restart_from_beginning();
      return url_parse_action::continue_;
    } else {
      *validation_error |= true;
      return std::unexpected(url_parse_errc::invalid_scheme_character);
    }

    return url_parse_action::increment;
  }

  auto parse_scheme(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (std::isalnum(byte, std::locale::classic()) || details::contains("+-."sv, byte)) {
      auto lower = std::tolower(byte, std::locale::classic());
      buffer.push_back(lower);
    } else if (byte == ':') {
      if (state_override) {
        if ((url.is_special() && !is_special(buffer)) || (!url.is_special() && is_special(buffer)) ||
            ((url.includes_credentials() || url.port) && (buffer == "file")) ||
            ((url.scheme == "file") && (!url.host || url.host.value().is_empty()))) {
          return std::unexpected(url_parse_errc::cannot_override_scheme);
        }
      }
      set_scheme_from_buffer();

      if (state_override) {
        if (url.port == default_port(url.scheme)) {
          clear_port();
        }
        return url_parse_action::success;
      }
      buffer.clear();

      if (url.scheme == "file") {
        if (!remaining_starts_with("//"sv)) {
          *validation_error |= true;
        }
        state = url_parse_state::file;
      } else if (url.is_special() && base && (base->scheme == url.scheme)) {
        state = url_parse_state::special_relative_or_authority;
      } else if (url.is_special()) {
        state = url_parse_state::special_authority_slashes;
      } else if (remaining_starts_with("/"sv)) {
        state = url_parse_state::path_or_authority;
        increment();
      } else {
        set_cannot_be_a_base_url_flag();
        add_empty_path_element();
        state = url_parse_state::cannot_be_a_base_url_path;
      }
    } else if (!state_override) {
      buffer.clear();
      state = url_parse_state::no_scheme;
      restart_from_beginning();
      return url_parse_action::continue_;
    } else {
      return std::unexpected(url_parse_errc::invalid_scheme_character);
    }

    return url_parse_action::increment;
  }

  auto parse_no_scheme(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (!base || (base->cannot_be_a_base_url && (byte != '#'))) {
      *validation_error |= true;
      return std::unexpected(url_parse_errc::not_an_absolute_url_with_fragment);
    } else if (base->cannot_be_a_base_url && (byte == '#')) {
      set_scheme_from_base();
      set_path_from_base();
      set_query_from_base();
      set_empty_fragment();
      set_cannot_be_a_base_url_flag();
      state = url_parse_state::fragment;
    } else if (base->scheme != "file") {
      state = url_parse_state::relative;
      restart_from_beginning();
      return url_parse_action::continue_;
    } else {
      state = url_parse_state::file;
      restart_from_beginning();
      return url_parse_action::continue_;
    }
    return url_parse_action::increment;
  }

  auto parse_special_relative_or_authority(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((byte == '/') && remaining_starts_with("/"sv)) {
      increment();
      state = url_parse_state::special_authority_ignore_slashes;
    } else {
      *validation_error |= true;
      decrement();
      state = url_parse_state::relative;
    }
    return url_parse_action::increment;
  }

  auto parse_path_or_authority(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (byte == '/') {
      state = url_parse_state::authority;
    } else {
      state = url_parse_state::path;
      decrement();
    }
    return url_parse_action::increment;
  }

  auto parse_relative(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    set_scheme_from_base();
    if (is_eof()) {
      set_authority_from_base();
      set_path_from_base();
      set_query_from_base();
    } else if (byte == '/') {
      state = url_parse_state::relative_slash;
    } else if (byte == '?') {
      set_authority_from_base();
      set_path_from_base();
      set_empty_query();
      state = url_parse_state::query;
    } else if (byte == '#') {
      set_authority_from_base();
      set_path_from_base();
      set_query_from_base();
      set_empty_fragment();
      state = url_parse_state::fragment;
    } else {
      if (url.is_special() && (byte == '\\')) {
        *validation_error |= true;
        state = url_parse_state::relative_slash;
      } else {
        set_authority_from_base();
        set_path_from_base();
        if (!url.path.empty()) {
          url.path.pop_back();
        }
        state = url_parse_state::path;

        if (input_it == begin(input)) {
          return url_parse_action::continue_;
        }

        decrement();
      }
    }

    return url_parse_action::increment;
  }

  auto parse_relative_slash(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (url.is_special() && ((byte == '/') || (byte == '\\'))) {
      if (byte == '\\') {
        *validation_error |= true;
      }
      state = url_parse_state::special_authority_ignore_slashes;
    } else if (byte == '/') {
      state = url_parse_state::authority;
    } else {
      set_authority_from_base();
      state = url_parse_state::path;
      decrement();
    }

    return url_parse_action::increment;
  }

  auto parse_special_authority_slashes(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((byte == '/') && remaining_starts_with("/"sv)) {
      increment();
      state = url_parse_state::special_authority_ignore_slashes;
    } else {
      *validation_error |= true;
      decrement();
      state = url_parse_state::special_authority_ignore_slashes;
    }

    return url_parse_action::increment;
  }

  auto parse_special_authority_ignore_slashes(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((byte != '/') && (byte != '\\')) {
      decrement();
      state = url_parse_state::authority;
    } else {
      *validation_error |= true;
    }
    return url_parse_action::increment;
  }

  auto parse_authority(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (byte == '@') {
      *validation_error |= true;
      if (at_flag) {
        buffer.insert(0, "%40");
      }
      at_flag = true;

      set_credentials_from_buffer();
      buffer.clear();
    } else if (((is_eof()) || (byte == '/') || (byte == '?') || (byte == '#')) ||
               (url.is_special() && (byte == '\\'))) {
      if (at_flag && buffer.empty()) {
        *validation_error |= true;
        return std::unexpected(url_parse_errc::empty_hostname);
      }
      restart_from_beginning_of_buffer();
      state = url_parse_state::host;
      buffer.clear();
      return url_parse_action::increment;
    } else {
      buffer.push_back(byte);
    }

    return url_parse_action::increment;
  }

  auto parse_hostname(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (state_override && (url.scheme == "file")) {
      state = url_parse_state::file_host;
      if (input_it == begin(input)) {
        return url_parse_action::continue_;
      }
      decrement();
    } else if ((byte == ':') && !square_braces_flag) {
      if (buffer.empty()) {
        *validation_error |= true;
        return std::unexpected(url_parse_errc::empty_hostname);
      }

      auto result = set_host_from_buffer();
      if (!result) {
        return std::unexpected(result.error());
      }
      buffer.clear();
      state = url_parse_state::port;

      if (state_override && (state_override.value() == url_parse_state::hostname)) {
        return url_parse_action::success;
      }
    } else if ((is_eof() || (byte == '/') || (byte == '?') || (byte == '#')) || (url.is_special() && (byte == '\\'))) {
      if (input_it != begin(input)) {
        decrement();
      }

      if (url.is_special() && buffer.empty()) {
        *validation_error |= true;
        return std::unexpected(url_parse_errc::empty_hostname);
      } else if (state_override && buffer.empty() && (url.includes_credentials() || url.port)) {
        *validation_error |= true;
        return url_parse_action::success;
      }

      auto result = set_host_from_buffer();
      if (!result) {
        return std::unexpected(result.error());
      }
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

  auto parse_port(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (std::isdigit(byte, std::locale::classic())) {
      buffer += byte;
    } else if (((is_eof()) || (byte == '/') || (byte == '?') || (byte == '#')) ||
               (url.is_special() && (byte == '\\')) || state_override) {
      auto result = set_port_from_buffer();
      if (!result) {
        return std::unexpected(result.error());
      }
      buffer.clear();

      if (state_override) {
        return url_parse_action::success;
      }

      decrement();
      state = url_parse_state::path_start;
    } else {
      *validation_error |= true;
      return std::unexpected(url_parse_errc::invalid_port);
    }

    return url_parse_action::increment;
  }

  auto parse_file(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    set_file_scheme();
    set_empty_host();

    if ((byte == '/') || (byte == '\\')) {
      if (byte == '\\') {
        *validation_error |= true;
      }
      state = url_parse_state::file_slash;
    } else if (base && (base->scheme == "file")) {
      set_host_from_base();
      set_path_from_base();
      set_query_from_base();
      if (byte == '?') {
        set_empty_query();
        state = url_parse_state::query;
      } else if (byte == '#') {
        set_empty_fragment();
        state = url_parse_state::fragment;
      } else {
        clear_query();
        if (!details::is_windows_drive_letter(still_to_process())) {
          details::shorten_path(url.scheme, url.path);
        } else {
          *validation_error |= true;
          clear_path();
        }
        state = url_parse_state::path;
        if (input_it == std::begin(input)) {
          return url_parse_action::continue_;
        }
        decrement();
      }
    } else {
      state = url_parse_state::path;
      if (input_it == std::begin(input)) {
        return url_parse_action::continue_;
      }
      decrement();
    }

    return url_parse_action::increment;
  }

  auto parse_file_slash(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((byte == '/') || (byte == '\\')) {
      if (byte == '\\') {
        *validation_error |= true;
      }
      state = url_parse_state::file_host;
    } else {
      if (base && (base->scheme == "file")) {
        set_host_from_base();
        if (!details::is_windows_drive_letter(still_to_process()) &&
            (!base->path.empty() && details::is_windows_drive_letter(base->path[0]))) {
          set_path_from_base0();
        }
      }

      state = url_parse_state::path;
      decrement();
    }

    return url_parse_action::increment;
  }

  auto parse_file_host(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((is_eof()) || (byte == '/') || (byte == '\\') || (byte == '?') || (byte == '#')) {
      bool at_begin = (input_it == begin(input));
      if (!at_begin) {
        decrement();
      }

      if (!state_override && details::is_windows_drive_letter(buffer)) {
        *validation_error |= true;
        state = url_parse_state::path;
      } else if (buffer.empty()) {
        set_empty_host();

        if (state_override) {
          return url_parse_action::success;
        }

        state = url_parse_state::path_start;
      } else {
        auto result = set_host_from_buffer();
        if (!result) {
          return std::unexpected(result.error());
        }
        if (url.host.value().serialize() == "localhost") {
          url.host = host{empty_host{}};
        }

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

  auto parse_path_start(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    bool at_begin = (input_it == begin(input));
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
      set_empty_query();
      state = url_parse_state::query;
    } else if (!state_override && (byte == '#')) {
      set_empty_fragment();
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

  auto parse_path(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (((is_eof()) || (byte == '/')) || (url.is_special() && (byte == '\\')) ||
        (!state_override && ((byte == '?') || (byte == '#')))) {
      if (url.is_special() && (byte == '\\')) {
        *validation_error |= true;
      }

      if (details::is_double_dot_path_segment(buffer)) {
        details::shorten_path(url.scheme, url.path);
        if (!((byte == '/') || (url.is_special() && (byte == '\\')))) {
          add_empty_path_element();
        }
      } else if (details::is_single_dot_path_segment(buffer) &&
                 !((byte == '/') || (url.is_special() && (byte == '\\')))) {
        add_empty_path_element();
      } else if (!details::is_single_dot_path_segment(buffer)) {
        if ((url.scheme == "file") && url.path.empty() && details::is_windows_drive_letter(buffer)) {
          if (!url.host || !url.host.value().is_empty()) {
            *validation_error |= true;
            set_empty_host();
          }
          buffer[1] = ':';
        }

        add_path_element_from_buffer();
      }

      buffer.clear();

      if ((url.scheme == "file") && (is_eof() || (byte == '?') || (byte == '#'))) {
        while ((url.path.size() > 1) && url.path[0].empty()) {
          *validation_error |= true;
          remove_path_element();
        }
      }

      if (byte == '?') {
        set_empty_query();
        state = url_parse_state::query;
      }

      if (byte == '#') {
        set_empty_fragment();
        state = url_parse_state::fragment;
      }
    } else {
      if (!details::is_url_code_point(byte) && (byte != '%')) {
        *validation_error |= true;
      }

      auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::path);
      buffer += pct_encoded.to_string();
    }

    return url_parse_action::increment;
  }

  auto parse_cannot_be_a_base_url(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (byte == '?') {
      encode_trailing_spaces_in_path0();
      set_empty_query();
      state = url_parse_state::query;
    } else if (byte == '#') {
      encode_trailing_spaces_in_path0();
      set_empty_fragment();
      state = url_parse_state::fragment;
    } else {
      if (!is_eof() && (!details::is_url_code_point(byte) && (byte != '%'))) {
        *validation_error |= true;
      } else if ((byte == '%') && !percent_encoding::is_percent_encoded(still_to_process())) {
        *validation_error |= true;
      }
      if (!is_eof()) {
        append_to_path0(byte);
      }
    }
    return url_parse_action::increment;
  }

  auto parse_query(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (!state_override && (byte == '#')) {
      set_empty_fragment();
      state = url_parse_state::fragment;
    } else if (!is_eof()) {
      if ((byte < '!') || (byte > '~') || (details::contains(R"("#<>)"sv, byte)) ||
          ((byte == '\'') && url.is_special())) {
        pct_encode_and_append_to_query(byte);
      } else {
        append_to_query(byte);
      }
    }
    return url_parse_action::increment;
  }

  auto parse_fragment(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (!is_eof()) {
      if (!details::is_url_code_point(byte) && (byte != '%')) {
        *validation_error |= true;
      }

      if ((byte == '%') && !percent_encoding::is_percent_encoded(still_to_process())) {
        *validation_error |= true;
      }

      append_to_fragment(byte);
    }
    return url_parse_action::increment;
  }

  void set_scheme_from_buffer() {
    url.scheme = buffer;
  }

  void set_file_scheme() {
    url.scheme = "file";
  }

  void set_scheme_from_base() {
    url.scheme = base->scheme;
  }

  void set_credentials_from_buffer() {
    auto password_token_seen_flag = false;
    for (auto c : buffer) {
      if ((c == ':') && !password_token_seen_flag) {
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
  }

  auto set_host_from_buffer() -> std::expected<void, url_parse_errc> {
    auto host = parse_host(buffer, !url.is_special(), validation_error);
    if (!host) {
      return std::unexpected(host.error());
    }
    url.host = host.value();
    return {};
  }

  void set_empty_host() {
    url.host = host{empty_host{}};
  }

  void set_host_from_base() {
    url.host = base->host;
  }

  auto set_port_from_buffer() -> std::expected<void, url_parse_errc> {
    if (!buffer.empty()) {
      auto port = details::port_number(buffer);

      if (!port) {
        *validation_error |= true;
        return std::unexpected(port.error());
      }

      auto dport = default_port(url.scheme);
      if (dport && (dport.value() == port.value())) {
        url.port = std::nullopt;
      } else {
        url.port = port.value();
      }
    }
    return {};
  }

  void clear_port() {
    url.port = std::nullopt;
  }

  void set_authority_from_base() {
    url.username = base->username;
    url.password = base->password;
    url.host = base->host;
    url.port = base->port;
  }

  void set_cannot_be_a_base_url_flag() {
    url.cannot_be_a_base_url = true;
  }

  void clear_path() {
    url.path.clear();
  }

  void add_empty_path_element() {
    url.path.emplace_back();
  }

  void add_path_element_from_buffer() {
    url.path.emplace_back(buffer);
  }

  void remove_path_element() {
    url.path.erase(url.path.begin());
  }

  void set_path_from_base() {
    url.path = base->path;
  }

  void set_path_from_base0() {
    url.path.push_back(base->path[0]);
  }

  void append_to_path0(char byte) {
    auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::c0_control);
    url.path[0] += pct_encoded.to_string();
  }

  void encode_trailing_spaces_in_path0() {
    if (url.path.empty()) {
      return;
    }
    auto& path = url.path[0];
    // Only encode the LAST space if it's trailing
    if (!path.empty() && path.back() == ' ') {
      path.pop_back();
      path += "%20";
    }
  }

  void clear_query() {
    url.query = std::nullopt;
  }

  void set_empty_query() {
    url.query = std::string();
  }

  void set_query_from_base() {
    url.query = base->query;
  }

  void pct_encode_and_append_to_query(char byte) {
    if (!url.query) {
      set_empty_query();
    }
    auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::any);
    url.query.value() += std::move(pct_encoded).to_string();
  }

  void append_to_query(char byte) {
    if (!url.query) {
      set_empty_query();
    }
    url.query.value().push_back(byte);
  }

  void set_empty_fragment() {
    url.fragment = std::string();
  }

  void append_to_fragment(char byte) {
    if (!url.fragment) {
      set_empty_fragment();
    }
    auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::fragment);
    url.fragment.value() += pct_encoded.to_string();
  }
};
}  // namespace skyr

#endif  // SKYR_CORE_URL_PARSER_CONTEXT_HPP
