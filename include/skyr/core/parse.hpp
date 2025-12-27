// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_CORE_PARSE_HPP
#define SKYR_CORE_PARSE_HPP

#include <system_error>
#include <optional>
#include <expected>
#include <skyr/core/url_record.hpp>
#include <skyr/core/errors.hpp>
#include <skyr/core/check_input.hpp>
#include <skyr/core/url_parser_context.hpp>

namespace skyr {
namespace details {
inline auto basic_parse(std::string_view input, bool *validation_error, const url_record *base, const url_record *url,
                        std::optional<url_parse_state> state_override) -> std::expected<url_record, url_parse_errc> {
  constexpr auto is_tab_or_newline = [](auto byte) { return (byte == '\t') || (byte == '\r') || (byte == '\n'); };

  if (url == nullptr) {
    input = remove_leading_c0_control_or_space(input, validation_error);
    input = remove_trailing_c0_control_or_space(input, validation_error);
  }

  auto context = url_parser_context(input, validation_error, base, url, state_override);
  while (true) {
    // remove tabs and new lines
    while (!context.is_eof() && is_tab_or_newline(context.next_byte())) {
      context.increment();
    }

    auto action = context.parse_next();
    if (!action) {
      return std::unexpected(action.error());
    }

    switch (action.value()) {
      case url_parse_action::success:
        return std::move(context).get_url();
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

  return std::move(context).get_url();
}

inline auto parse(std::string_view input, bool *validation_error, const url_record *base)
    -> std::expected<url_record, url_parse_errc> {
  auto url = basic_parse(input, validation_error, base, nullptr, std::nullopt);

  if (!url) {
    return url;
  }

  if (url.value().scheme == "blob") {
    return url;
  }

  if (url.value().path.empty()) {
    return url;
  }

  return url;
}
}  // namespace details

inline auto parse(std::string_view input) -> std::expected<url_record, url_parse_errc> {
  bool validation_error = false;
  return details::parse(input, &validation_error, nullptr);
}

inline auto parse(std::string_view input, bool *validation_error) -> std::expected<url_record, url_parse_errc> {
  return details::parse(input, validation_error, nullptr);
}

inline auto parse(std::string_view input, const url_record &base) -> std::expected<url_record, url_parse_errc> {
  bool validation_error = false;
  return details::parse(input, &validation_error, &base);
}

inline auto parse(std::string_view input, const url_record &base, bool *validation_error)
    -> std::expected<url_record, url_parse_errc> {
  return details::parse(input, validation_error, &base);
}
}  // namespace skyr

#endif  // SKYR_CORE_PARSE_HPP
