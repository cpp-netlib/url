// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CORE_URL_PARSER_CONTEXT_HPP
#define SKYR_V1_CORE_URL_PARSER_CONTEXT_HPP

#include <cassert>
#include <string_view>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/v1/core/errors.hpp>
#include <skyr/v1/core/url_record.hpp>
#include "url_parse_impl.hpp"

namespace skyr {
inline namespace v1 {
enum class url_parse_action {
  success = 0,
  increment,
  continue_,
};

class url_parser_context {

 private:

  std::string_view input;

 public:

  std::string_view::const_iterator it;

  bool *validation_error;

  const url_record *base;
  url_record url;

  url_parse_state state;
  std::optional<url_parse_state> state_override;

  std::string buffer;

  bool at_flag;
  bool square_braces_flag;
  bool password_token_seen_flag;

  url_parser_context(
      std::string_view input,
      bool *validation_error,
      const url_record *base,
      const url_record *url,
      std::optional<url_parse_state> state_override);

  [[nodiscard]] auto is_eof() const noexcept {
    return it == end(input);
  }

  void increment() noexcept {
    assert(it != end(input));
    ++it;
  }

  void decrement() noexcept {
    assert(it != begin(input));
    --it;
  }

  void reset() noexcept {
    it = begin(input);
  }

  void restart_from_buffer() noexcept {
    it = it - buffer.size() - 1;
  }

  auto parse_scheme_start(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_scheme(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_no_scheme(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_special_relative_or_authority(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_path_or_authority(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_relative(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_relative_slash(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_special_authority_slashes(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_special_authority_ignore_slashes(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_authority(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_hostname(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_port(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_file(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_file_slash(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_file_host(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_path_start(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_path(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_cannot_be_a_base_url(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_query(char byte) -> tl::expected<url_parse_action, url_parse_errc>;
  auto parse_fragment(char byte) -> tl::expected<url_parse_action, url_parse_errc>;

};
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_CORE_URL_PARSER_CONTEXT_HPP
