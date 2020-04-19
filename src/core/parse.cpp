// Copyright 2016-2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <skyr/core/parse.hpp>
#include <skyr/core/errors.hpp>
#include "url_parse_impl.hpp"
#include "url_parser_context.hpp"

namespace skyr {
inline namespace v1 {
namespace details {
auto basic_parse(
    std::string_view input,
    const url_record *base,
    const url_record *url,
    std::optional<url_parse_state> state_override) -> tl::expected<url_record, std::error_code> {
  using return_type = tl::expected<url_parse_action, url_parse_errc>;
  using url_parse_func = std::function<return_type(url_parser_context &, char)>;
  using url_parse_funcs = std::vector<url_parse_func>;

  auto parse_funcs = url_parse_funcs{
      [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_scheme_start(byte);
       },
      [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_scheme(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_no_scheme(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_special_relative_or_authority(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_path_or_authority(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_relative(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_relative_slash(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_special_authority_slashes(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_special_authority_ignore_slashes(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_authority(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_hostname(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_hostname(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_port(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_file(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_file_slash(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_file_host(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_path_start(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_path(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_cannot_be_a_base_url(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_query(byte);
       },
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_fragment(byte);
       }
  };

  auto context = url_parser_context(
      input, base, url, state_override);

  while (true) {
    auto func = parse_funcs[static_cast<std::size_t>(context.state)];
    auto byte = context.is_eof() ? '\0' : *context.it;
    auto action = func(context, byte);
    if (!action) {
      return tl::make_unexpected(make_error_code(action.error()));
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
    std::string_view input) -> tl::expected<url_record, std::error_code> {
  return details::parse(input, nullptr);
}

auto parse(
    std::string_view input,
    const url_record &base) -> tl::expected<url_record, std::error_code> {
  return details::parse(input, &base);
}
}  // namespace v1
}  // namespace skyr
