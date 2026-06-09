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
constexpr auto contains(std::string_view view, char element) noexcept {
  auto first = std::cbegin(view);
  auto last = std::cend(view);
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

constexpr auto is_windows_drive_letter(std::string_view segment) noexcept {
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

constexpr auto is_single_dot_path_segment(std::string_view segment) noexcept {
  return (segment == ".") || (segment == "%2e") || (segment == "%2E");
}

constexpr auto is_double_dot_path_segment(std::string_view segment) noexcept {
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

enum class url_parse_action : std::uint8_t {
  success = 0,
  increment,
  continue_,
};

class url_parser_context {
 private:
  url_record url_;
  url_parse_state state_;
  std::string_view input_{};
  std::string_view::const_iterator input_it_{};
  bool* validation_error_;
  const url_record* base_;
  std::optional<url_parse_state> state_override_{};
  std::string buffer_{};

  bool at_flag_;
  bool square_braces_flag_;

 public:
  url_parser_context(std::string_view input, bool* validation_error, const url_record* base, const url_record* url,
                     std::optional<url_parse_state> state_override)
      : url_(url ? *url : url_record{})
      , state_(state_override ? state_override.value() : url_parse_state::scheme_start)
      , input_(input)
      , input_it_(begin(input_))
      , validation_error_(validation_error)
      , base_(base)
      , state_override_(state_override)
      , buffer_()
      , at_flag_(false)
      , square_braces_flag_(false) {
  }

  [[nodiscard]] auto get_url() && -> url_record&& {
    return std::move(url_);
  }

  [[nodiscard]] auto is_eof() const noexcept {
    return input_it_ == std::end(input_);
  }

  [[nodiscard]] auto next_byte() const noexcept {
    return !is_eof() ? *input_it_ : '\0';
  }

  void increment() noexcept {
    assert(input_it_ != std::end(input_));
    ++input_it_;
  }

  auto parse_next() -> std::expected<url_parse_action, url_parse_errc> {
    auto byte = next_byte();
    switch (state_) {
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
    assert(input_it_ != std::begin(input_));
    --input_it_;
  }

  void restart_from_beginning() noexcept {
    input_it_ = std::begin(input_);
  }

  void restart_from_beginning_of_buffer() noexcept {
    input_it_ -= (buffer_.size() + 1);
  }

  [[nodiscard]] auto still_to_process() const noexcept -> std::string_view {
    return input_.substr(std::distance(std::begin(input_), input_it_));
  }

  [[nodiscard]] auto remaining_starts_with(std::string_view chars) const noexcept -> bool {
    return !still_to_process().empty() && still_to_process().substr(1).starts_with(chars);
  }

  auto parse_scheme_start(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (std::isalpha(byte, std::locale::classic())) {
      auto lower = std::tolower(byte, std::locale::classic());
      buffer_.push_back(lower);
      state_ = url_parse_state::scheme;
    } else if (!state_override_) {
      state_ = url_parse_state::no_scheme;
      restart_from_beginning();
      return url_parse_action::continue_;
    } else {
      *validation_error_ |= true;
      return std::unexpected(url_parse_errc::invalid_scheme_character);
    }

    return url_parse_action::increment;
  }

  auto parse_scheme(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (std::isalnum(byte, std::locale::classic()) || details::contains("+-."sv, byte)) {
      auto lower = std::tolower(byte, std::locale::classic());
      buffer_.push_back(lower);
    } else if (byte == ':') {
      if (state_override_) {
        if ((url_.is_special() && !is_special(buffer_)) || (!url_.is_special() && is_special(buffer_)) ||
            ((url_.includes_credentials() || url_.port) && (buffer_ == "file")) ||
            ((url_.scheme == "file") && (!url_.host || url_.host.value().is_empty()))) {
          return std::unexpected(url_parse_errc::cannot_override_scheme);
        }
      }
      set_scheme_from_buffer();

      if (state_override_) {
        if (url_.port == default_port(url_.scheme)) {
          clear_port();
        }
        return url_parse_action::success;
      }
      buffer_.clear();

      if (url_.scheme == "file") {
        if (!remaining_starts_with("//"sv)) {
          *validation_error_ |= true;
        }
        state_ = url_parse_state::file;
      } else if (url_.is_special() && base_ && (base_->scheme == url_.scheme)) {
        state_ = url_parse_state::special_relative_or_authority;
      } else if (url_.is_special()) {
        state_ = url_parse_state::special_authority_slashes;
      } else if (remaining_starts_with("/"sv)) {
        state_ = url_parse_state::path_or_authority;
        increment();
      } else {
        set_cannot_be_a_base_url_flag();
        add_empty_path_element();
        state_ = url_parse_state::cannot_be_a_base_url_path;
      }
    } else if (!state_override_) {
      buffer_.clear();
      state_ = url_parse_state::no_scheme;
      restart_from_beginning();
      return url_parse_action::continue_;
    } else {
      return std::unexpected(url_parse_errc::invalid_scheme_character);
    }

    return url_parse_action::increment;
  }

  auto parse_no_scheme(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (!base_ || (base_->cannot_be_a_base_url && (byte != '#'))) {
      *validation_error_ |= true;
      return std::unexpected(url_parse_errc::not_an_absolute_url_with_fragment);
    } else if (base_->cannot_be_a_base_url && (byte == '#')) {
      set_scheme_from_base();
      set_path_from_base();
      set_query_from_base();
      set_empty_fragment();
      set_cannot_be_a_base_url_flag();
      state_ = url_parse_state::fragment;
    } else if (base_->scheme != "file") {
      state_ = url_parse_state::relative;
      restart_from_beginning();
      return url_parse_action::continue_;
    } else {
      state_ = url_parse_state::file;
      restart_from_beginning();
      return url_parse_action::continue_;
    }
    return url_parse_action::increment;
  }

  auto parse_special_relative_or_authority(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((byte == '/') && remaining_starts_with("/"sv)) {
      increment();
      state_ = url_parse_state::special_authority_ignore_slashes;
    } else {
      *validation_error_ |= true;
      decrement();
      state_ = url_parse_state::relative;
    }
    return url_parse_action::increment;
  }

  auto parse_path_or_authority(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (byte == '/') {
      state_ = url_parse_state::authority;
    } else {
      state_ = url_parse_state::path;
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
      state_ = url_parse_state::relative_slash;
    } else if (byte == '?') {
      set_authority_from_base();
      set_path_from_base();
      set_empty_query();
      state_ = url_parse_state::query;
    } else if (byte == '#') {
      set_authority_from_base();
      set_path_from_base();
      set_query_from_base();
      set_empty_fragment();
      state_ = url_parse_state::fragment;
    } else {
      if (url_.is_special() && (byte == '\\')) {
        *validation_error_ |= true;
        state_ = url_parse_state::relative_slash;
      } else {
        set_authority_from_base();
        set_path_from_base();
        if (!url_.path.empty()) {
          url_.path.pop_back();
        }
        state_ = url_parse_state::path;

        if (input_it_ == begin(input_)) {
          return url_parse_action::continue_;
        }

        decrement();
      }
    }

    return url_parse_action::increment;
  }

  auto parse_relative_slash(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (url_.is_special() && ((byte == '/') || (byte == '\\'))) {
      if (byte == '\\') {
        *validation_error_ |= true;
      }
      state_ = url_parse_state::special_authority_ignore_slashes;
    } else if (byte == '/') {
      state_ = url_parse_state::authority;
    } else {
      set_authority_from_base();
      state_ = url_parse_state::path;
      decrement();
    }

    return url_parse_action::increment;
  }

  auto parse_special_authority_slashes(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((byte == '/') && remaining_starts_with("/"sv)) {
      increment();
      state_ = url_parse_state::special_authority_ignore_slashes;
    } else {
      *validation_error_ |= true;
      decrement();
      state_ = url_parse_state::special_authority_ignore_slashes;
    }

    return url_parse_action::increment;
  }

  auto parse_special_authority_ignore_slashes(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((byte != '/') && (byte != '\\')) {
      decrement();
      state_ = url_parse_state::authority;
    } else {
      *validation_error_ |= true;
    }
    return url_parse_action::increment;
  }

  auto parse_authority(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (byte == '@') {
      *validation_error_ |= true;
      if (at_flag_) {
        // Subsequent @ characters
        // If we already have a password (from first @ segment containing ':'),
        // append to password. Otherwise, parse normally as username:password.
        if (!url_.password.empty()) {
          url_.password += "%40";
          for (auto c : buffer_) {
            auto pct_encoded = percent_encode_byte(std::byte(c), percent_encoding::encode_set::userinfo);
            url_.password += pct_encoded.to_string();
          }
          buffer_.clear();
        } else {
          buffer_.insert(0, "%40");
          set_credentials_from_buffer();
          buffer_.clear();
        }
      } else {
        at_flag_ = true;
        set_credentials_from_buffer();
        buffer_.clear();
      }
    } else if (((is_eof()) || (byte == '/') || (byte == '?') || (byte == '#')) ||
               (url_.is_special() && (byte == '\\'))) {
      if (at_flag_ && buffer_.empty()) {
        *validation_error_ |= true;
        return std::unexpected(url_parse_errc::empty_hostname);
      }
      restart_from_beginning_of_buffer();
      state_ = url_parse_state::host;
      buffer_.clear();
      return url_parse_action::increment;
    } else {
      buffer_.push_back(byte);
    }

    return url_parse_action::increment;
  }

  auto parse_hostname(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (state_override_ && (url_.scheme == "file")) {
      state_ = url_parse_state::file_host;
      if (input_it_ == begin(input_)) {
        return url_parse_action::continue_;
      }
      decrement();
    } else if ((byte == ':') && !square_braces_flag_) {
      if (buffer_.empty()) {
        *validation_error_ |= true;
        return std::unexpected(url_parse_errc::empty_hostname);
      }

      auto result = set_host_from_buffer();
      if (!result) {
        return std::unexpected(result.error());
      }
      buffer_.clear();
      state_ = url_parse_state::port;

      if (state_override_ && (state_override_.value() == url_parse_state::hostname)) {
        return url_parse_action::success;
      }
    } else if ((is_eof() || (byte == '/') || (byte == '?') || (byte == '#')) || (url_.is_special() && (byte == '\\'))) {
      if (input_it_ != begin(input_)) {
        decrement();
      }

      if (url_.is_special() && buffer_.empty()) {
        *validation_error_ |= true;
        return std::unexpected(url_parse_errc::empty_hostname);
      } else if (state_override_ && buffer_.empty() && (url_.includes_credentials() || url_.port)) {
        *validation_error_ |= true;
        return url_parse_action::success;
      }

      auto result = set_host_from_buffer();
      if (!result) {
        return std::unexpected(result.error());
      }
      buffer_.clear();
      state_ = url_parse_state::path_start;

      if (state_override_) {
        return url_parse_action::success;
      }
    } else {
      if (byte == '[') {
        square_braces_flag_ = true;
      } else if (byte == ']') {
        square_braces_flag_ = false;
      }
      buffer_ += byte;
    }
    return url_parse_action::increment;
  }

  auto parse_port(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (std::isdigit(byte, std::locale::classic())) {
      buffer_ += byte;
    } else if (((is_eof()) || (byte == '/') || (byte == '?') || (byte == '#')) ||
               (url_.is_special() && (byte == '\\')) || state_override_) {
      auto result = set_port_from_buffer();
      if (!result) {
        return std::unexpected(result.error());
      }
      buffer_.clear();

      if (state_override_) {
        return url_parse_action::success;
      }

      decrement();
      state_ = url_parse_state::path_start;
    } else {
      *validation_error_ |= true;
      return std::unexpected(url_parse_errc::invalid_port);
    }

    return url_parse_action::increment;
  }

  auto parse_file(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    set_file_scheme();
    set_empty_host();

    if ((byte == '/') || (byte == '\\')) {
      if (byte == '\\') {
        *validation_error_ |= true;
      }
      state_ = url_parse_state::file_slash;
    } else if (base_ && (base_->scheme == "file")) {
      set_host_from_base();
      set_path_from_base();
      set_query_from_base();
      if (byte == '?') {
        set_empty_query();
        state_ = url_parse_state::query;
      } else if (byte == '#') {
        set_empty_fragment();
        state_ = url_parse_state::fragment;
      } else {
        clear_query();
        if (!details::is_windows_drive_letter(still_to_process())) {
          details::shorten_path(url_.scheme, url_.path);
        } else {
          *validation_error_ |= true;
          clear_path();
        }
        state_ = url_parse_state::path;
        if (input_it_ == std::begin(input_)) {
          return url_parse_action::continue_;
        }
        decrement();
      }
    } else {
      state_ = url_parse_state::path;
      if (input_it_ == std::begin(input_)) {
        return url_parse_action::continue_;
      }
      decrement();
    }

    return url_parse_action::increment;
  }

  auto parse_file_slash(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((byte == '/') || (byte == '\\')) {
      if (byte == '\\') {
        *validation_error_ |= true;
      }
      state_ = url_parse_state::file_host;
    } else {
      if (base_ && (base_->scheme == "file")) {
        set_host_from_base();
        if (!details::is_windows_drive_letter(still_to_process()) &&
            (!base_->path.empty() && details::is_windows_drive_letter(base_->path[0]))) {
          set_path_from_base0();
        }
      }

      state_ = url_parse_state::path;
      decrement();
    }

    return url_parse_action::increment;
  }

  auto parse_file_host(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if ((is_eof()) || (byte == '/') || (byte == '\\') || (byte == '?') || (byte == '#')) {
      bool at_begin = (input_it_ == begin(input_));
      if (!at_begin) {
        decrement();
      }

      if (!state_override_ && details::is_windows_drive_letter(buffer_)) {
        *validation_error_ |= true;
        state_ = url_parse_state::path;
      } else if (buffer_.empty()) {
        set_empty_host();

        if (state_override_) {
          return url_parse_action::success;
        }

        state_ = url_parse_state::path_start;
      } else {
        auto result = set_host_from_buffer();
        if (!result) {
          return std::unexpected(result.error());
        }
        if (url_.host.value().serialize() == "localhost") {
          url_.host = host{empty_host{}};
        }

        if (state_override_) {
          return url_parse_action::success;
        }

        buffer_.clear();

        state_ = url_parse_state::path_start;
      }
    } else {
      buffer_.push_back(byte);
    }

    return url_parse_action::increment;
  }

  auto parse_path_start(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    bool at_begin = (input_it_ == begin(input_));
    if (url_.is_special()) {
      if (byte == '\\') {
        *validation_error_ |= true;
      }
      state_ = url_parse_state::path;
      if ((byte != '/') && (byte != '\\')) {
        if (at_begin) {
          return url_parse_action::continue_;
        }
        decrement();
      }
    } else if (!state_override_ && (byte == '?')) {
      set_empty_query();
      state_ = url_parse_state::query;
    } else if (!state_override_ && (byte == '#')) {
      set_empty_fragment();
      state_ = url_parse_state::fragment;
    } else if (!is_eof()) {
      state_ = url_parse_state::path;
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
    if (((is_eof()) || (byte == '/')) || (url_.is_special() && (byte == '\\')) ||
        (!state_override_ && ((byte == '?') || (byte == '#')))) {
      if (url_.is_special() && (byte == '\\')) {
        *validation_error_ |= true;
      }

      if (details::is_double_dot_path_segment(buffer_)) {
        details::shorten_path(url_.scheme, url_.path);
        if (!((byte == '/') || (url_.is_special() && (byte == '\\')))) {
          add_empty_path_element();
        }
      } else if (details::is_single_dot_path_segment(buffer_) &&
                 !((byte == '/') || (url_.is_special() && (byte == '\\')))) {
        add_empty_path_element();
      } else if (!details::is_single_dot_path_segment(buffer_)) {
        if ((url_.scheme == "file") && url_.path.empty() && details::is_windows_drive_letter(buffer_)) {
          // For file URLs with Windows drive letters, the host should be empty
          // UNLESS it was inherited from a file base URL (per WPT test expectations)
          bool inherited_from_file_base = (base_ && base_->scheme == "file");
          if (!inherited_from_file_base && (!url_.host || !url_.host.value().is_empty())) {
            *validation_error_ |= true;
            set_empty_host();
          }
          buffer_[1] = ':';
        }

        add_path_element_from_buffer();
      }

      buffer_.clear();

      if ((url_.scheme == "file") && (is_eof() || (byte == '?') || (byte == '#'))) {
        while ((url_.path.size() > 1) && url_.path[0].empty()) {
          *validation_error_ |= true;
          remove_path_element();
        }
      }

      if (byte == '?') {
        set_empty_query();
        state_ = url_parse_state::query;
      }

      if (byte == '#') {
        set_empty_fragment();
        state_ = url_parse_state::fragment;
      }
    } else {
      if (!details::is_url_code_point(byte) && (byte != '%')) {
        *validation_error_ |= true;
      }

      auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::path);
      buffer_ += pct_encoded.to_string();
    }

    return url_parse_action::increment;
  }

  auto parse_cannot_be_a_base_url(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (byte == '?') {
      encode_trailing_spaces_in_path0();
      set_empty_query();
      state_ = url_parse_state::query;
    } else if (byte == '#') {
      encode_trailing_spaces_in_path0();
      set_empty_fragment();
      state_ = url_parse_state::fragment;
    } else {
      if (!is_eof() && (!details::is_url_code_point(byte) && (byte != '%'))) {
        *validation_error_ |= true;
      } else if ((byte == '%') && !percent_encoding::is_percent_encoded(still_to_process())) {
        *validation_error_ |= true;
      }
      if (!is_eof()) {
        append_to_path0(byte);
      }
    }
    return url_parse_action::increment;
  }

  auto parse_query(char byte) -> std::expected<url_parse_action, url_parse_errc> {
    if (!state_override_ && (byte == '#')) {
      set_empty_fragment();
      state_ = url_parse_state::fragment;
    } else if (!is_eof()) {
      if ((byte < '!') || (byte > '~') || (details::contains(R"("#<>)"sv, byte)) ||
          ((byte == '\'') && url_.is_special())) {
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
        *validation_error_ |= true;
      }

      if ((byte == '%') && !percent_encoding::is_percent_encoded(still_to_process())) {
        *validation_error_ |= true;
      }

      append_to_fragment(byte);
    }
    return url_parse_action::increment;
  }

  void set_scheme_from_buffer() {
    url_.scheme = buffer_;
  }

  void set_file_scheme() {
    url_.scheme = "file";
  }

  void set_scheme_from_base() {
    url_.scheme = base_->scheme;
  }

  void set_credentials_from_buffer() {
    auto password_token_seen_flag = false;
    for (auto c : buffer_) {
      if ((c == ':') && !password_token_seen_flag) {
        password_token_seen_flag = true;
        continue;
      }

      auto pct_encoded = percent_encode_byte(std::byte(c), percent_encoding::encode_set::userinfo);
      if (password_token_seen_flag) {
        url_.password += pct_encoded.to_string();
      } else {
        url_.username += pct_encoded.to_string();
      }
    }
  }

  auto set_host_from_buffer() -> std::expected<void, url_parse_errc> {
    auto new_host = parse_host(buffer_, !url_.is_special(), validation_error_);
    if (!new_host) {
      return std::unexpected(new_host.error());
    }
    url_.host = new_host.value();
    return {};
  }

  void set_empty_host() {
    url_.host = host{empty_host{}};
  }

  void set_host_from_base() {
    url_.host = base_->host;
  }

  auto set_port_from_buffer() -> std::expected<void, url_parse_errc> {
    if (!buffer_.empty()) {
      auto port = details::port_number(buffer_);

      if (!port) {
        *validation_error_ |= true;
        return std::unexpected(port.error());
      }

      auto dport = default_port(url_.scheme);
      if (dport && (dport.value() == port.value())) {
        url_.port = std::nullopt;
      } else {
        url_.port = port.value();
      }
    }
    return {};
  }

  void clear_port() {
    url_.port = std::nullopt;
  }

  void set_authority_from_base() {
    url_.username = base_->username;
    url_.password = base_->password;
    url_.host = base_->host;
    url_.port = base_->port;
  }

  void set_cannot_be_a_base_url_flag() {
    url_.cannot_be_a_base_url = true;
  }

  void clear_path() {
    url_.path.clear();
  }

  void add_empty_path_element() {
    url_.path.emplace_back();
  }

  void add_path_element_from_buffer() {
    url_.path.emplace_back(buffer_);
  }

  void remove_path_element() {
    url_.path.erase(url_.path.begin());
  }

  void set_path_from_base() {
    url_.path = base_->path;
  }

  void set_path_from_base0() {
    url_.path.push_back(base_->path[0]);
  }

  void append_to_path0(char byte) {
    auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::c0_control);
    url_.path[0] += pct_encoded.to_string();
  }

  void encode_trailing_spaces_in_path0() {
    if (url_.path.empty()) {
      return;
    }
    auto& path = url_.path[0];
    // Only encode the LAST space if it's trailing
    if (!path.empty() && path.back() == ' ') {
      path.pop_back();
      path += "%20";
    }
  }

  void clear_query() {
    url_.query = std::nullopt;
  }

  void set_empty_query() {
    url_.query = std::string();
  }

  void set_query_from_base() {
    url_.query = base_->query;
  }

  void pct_encode_and_append_to_query(char byte) {
    if (!url_.query) {
      set_empty_query();
    }
    auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::any);
    url_.query.value() += std::move(pct_encoded).to_string();
  }

  void append_to_query(char byte) {
    if (!url_.query) {
      set_empty_query();
    }
    url_.query.value().push_back(byte);
  }

  void set_empty_fragment() {
    url_.fragment = std::string();
  }

  void append_to_fragment(char byte) {
    if (!url_.fragment) {
      set_empty_fragment();
    }
    auto pct_encoded = percent_encode_byte(std::byte(byte), percent_encoding::encode_set::fragment);
    url_.fragment.value() += pct_encoded.to_string();
  }
};
}  // namespace skyr

#endif  // SKYR_CORE_URL_PARSER_CONTEXT_HPP
