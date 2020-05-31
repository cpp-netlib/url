// Copyright 2016-2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/v1/core/parse.hpp>
#include <skyr/v1/core/check_input.hpp>
#include <skyr/v1/core/errors.hpp>
#include "url_parse_impl.hpp"
#include "url_parser_context.hpp"

namespace skyr {
inline namespace v1 {
namespace {
auto parse_next(url_parser_context &context, char byte)
    -> tl::expected<url_parse_action, url_parse_errc> {

  switch (context.state) {
    case url_parse_state::scheme_start:
      return context.parse_scheme_start(byte);
    case url_parse_state::scheme:
      return context.parse_scheme(byte);
    case url_parse_state::no_scheme:
      return context.parse_no_scheme(byte);
    case url_parse_state::special_relative_or_authority:
      return context.parse_special_relative_or_authority(byte);
    case url_parse_state::path_or_authority:
      return context.parse_path_or_authority(byte);
    case url_parse_state::relative:
      return context.parse_relative(byte);
    case url_parse_state::relative_slash:
      return context.parse_relative_slash(byte);
    case url_parse_state::special_authority_slashes:
      return context.parse_special_authority_slashes(byte);
    case url_parse_state::special_authority_ignore_slashes:
      return context.parse_special_authority_ignore_slashes(byte);
    case url_parse_state::authority:
      return context.parse_authority(byte);
    case url_parse_state::hostname:
    case url_parse_state::host:
      return context.parse_hostname(byte);
    case url_parse_state::port:
      return context.parse_port(byte);
    case url_parse_state::file:
      return context.parse_file(byte);
    case url_parse_state::file_slash:
      return context.parse_file_slash(byte);
    case url_parse_state::file_host:
      return context.parse_file_host(byte);
    case url_parse_state::path_start:
      return context.parse_path_start(byte);
    case url_parse_state::path:
      return context.parse_path(byte);
    case url_parse_state::cannot_be_a_base_url_path:
      return context.parse_cannot_be_a_base_url(byte);
    case url_parse_state::query:
      return context.parse_query(byte);
    case url_parse_state::fragment:
      return context.parse_fragment(byte);
    default:
      return tl::make_unexpected(url_parse_errc::cannot_override_scheme);
  }
}
}

namespace details {
auto basic_parse(
    std::string_view input,
    bool *validation_error,
    const url_record *base,
    const url_record *url,
    std::optional<url_parse_state> state_override) -> tl::expected<url_record, url_parse_errc> {
  if (url == nullptr) {
    input = remove_leading_c0_control_or_space(input, validation_error);
    input = remove_trailing_c0_control_or_space(input, validation_error);
  }
  auto input_ = std::string(input);
  remove_tabs_and_newlines(input_, validation_error);

  auto context = url_parser_context(input_, validation_error, base, url, state_override);

  while (true) {
    auto byte = context.is_eof() ? '\0' : *context.it;
    auto action = parse_next(context, byte);
    if (!action) {
      return tl::make_unexpected(action.error());
    }

    switch (action.value()) {
      case url_parse_action::success:
        return context.url;
      case url_parse_action::increment:
        break;
      case url_parse_action::continue_:
        continue;
    }

    if (context.is_eof()) {
      break;
    }
    context.increment();
  }

  return context.url;
}
}  // namespace details

auto parse(
    std::string_view input) -> tl::expected<url_record, url_parse_errc> {
  bool validation_error = false;
  return details::parse(input, &validation_error, nullptr);
}

auto parse(
    std::string_view input,
    bool *validation_error) -> tl::expected<url_record, url_parse_errc> {
  return details::parse(input, validation_error, nullptr);
}

auto parse(
    std::string_view input,
    const url_record &base) -> tl::expected<url_record, url_parse_errc> {
  bool validation_error = false;
  return details::parse(input, &validation_error, &base);
}

auto parse(
    std::string_view input,
    const url_record &base,
    bool *validation_error) -> tl::expected<url_record, url_parse_errc> {
  return details::parse(input, validation_error, &base);
}
}  // namespace v1
}  // namespace skyr
