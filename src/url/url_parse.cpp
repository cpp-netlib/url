// Copyright 2016-2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <deque>
#include <map>
#include "skyr/url/url_parse.hpp"
#include "skyr/url/url_error.hpp"
#include "skyr/url/url_serialize.hpp"
#include "url_parse_impl.hpp"
#include "url_parser_context.hpp"

namespace skyr {
inline namespace v1 {
namespace details {
tl::expected<url_record, std::error_code> basic_parse(
    std::string_view input,
    std::optional<url_record> base,
    const std::optional<url_record> &url,
    std::optional<url_parse_state> state_override) {
  using return_type = tl::expected<url_parse_action, url_parse_errc>;
  using url_parse_func = std::function<return_type(url_parser_context &, char)>;
  using url_parse_funcs = std::map<url_parse_state, url_parse_func>;

  auto parse_funcs = url_parse_funcs{
      {url_parse_state::scheme_start,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_scheme_start(byte);
       }},
      {url_parse_state::scheme,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_scheme(byte);
       }},
      {url_parse_state::no_scheme,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_no_scheme(byte);
       }},
      {url_parse_state::special_relative_or_authority,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_special_relative_or_authority(byte);
       }},
      {url_parse_state::path_or_authority,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_path_or_authority(byte);
       }},
      {url_parse_state::relative,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_relative(byte);
       }},
      {url_parse_state::relative_slash,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_relative_slash(byte);
       }},
      {url_parse_state::special_authority_slashes,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_special_authority_slashes(byte);
       }},
      {url_parse_state::special_authority_ignore_slashes,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_special_authority_ignore_slashes(byte);
       }},
      {url_parse_state::authority,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_authority(byte);
       }},
      {url_parse_state::host,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_hostname(byte);
       }},
      {url_parse_state::hostname,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_hostname(byte);
       }},
      {url_parse_state::port,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_port(byte);
       }},
      {url_parse_state::file,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_file(byte);
       }},
      {url_parse_state::file_slash,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_file_slash(byte);
       }},
      {url_parse_state::file_host,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_file_host(byte);
       }},
      {url_parse_state::path_start,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_path_start(byte);
       }},
      {url_parse_state::path,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_path(byte);
       }},
      {url_parse_state::cannot_be_a_base_url_path,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_cannot_be_a_base_url(byte);
       }},
      {url_parse_state::query,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_query(byte);
       }},
      {url_parse_state::fragment,
       [](auto &context,
          auto byte) -> tl::expected<url_parse_action, url_parse_errc> {
         return context.parse_fragment(byte);
       }}};

  auto context = url_parser_context(
      input, std::move(base), url, state_override);

  while (true) {
    auto func = parse_funcs[context.state];
    auto byte = context.is_eof() ? static_cast<char>(0) : *context.it;
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

tl::expected<url_record, std::error_code> parse(
    std::string_view input,
    std::optional<url_record> base) {
  auto url = details::basic_parse(input, std::move(base));

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
}  // namespace v1
}  // namespace skyr
