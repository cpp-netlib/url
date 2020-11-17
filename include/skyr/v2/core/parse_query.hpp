// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_CORE_PARSE_QUERY_HPP
#define SKYR_V2_CORE_PARSE_QUERY_HPP

#include <string>
#include <vector>
#include <range/v3/view/split_when.hpp>
#include <range/v3/view/transform.hpp>
#include <skyr/v2/core/parse.hpp>
#include <skyr/v2/percent_encoding/percent_decode.hpp>

namespace skyr::inline v2 {
///
struct query_parameter {
  std::string name;
  std::optional<std::string> value;

  /// Constructor
  query_parameter() = default;

  /// Constructor
  /// \param name The parameter name
  query_parameter(std::string name) : name(std::move(name)) {}

  /// Constructor
  /// \param name The parameter name
  /// \param value The parameter value
  query_parameter(std::string name, std::string value) : name(std::move(name)), value(std::move(value)) {}
};


inline auto parse_query(
    std::string_view input, bool *validation_error) -> tl::expected<std::vector<query_parameter>, url_parse_errc> {
  if (!input.empty() && (input.front() == '?')) {
    input.remove_prefix(1);
  }

  auto url = details::basic_parse(input, validation_error, nullptr, nullptr, url_parse_state::query);
  if (url) {
    static constexpr auto is_separator = [](auto &&c) { return c == '&' || c == ';'; };

    static constexpr auto to_nvp = [](auto &&param) -> query_parameter {
      auto element = std::string_view(std::addressof(*ranges::begin(param)), ranges::distance(param));
      auto delim = element.find_first_of('=');
      if (delim != std::string_view::npos) {
        return {std::string(element.substr(0, delim)), std::string(element.substr(delim + 1))};
      } else {
        return {std::string(element)};
      }
    };

    std::vector<query_parameter> parameters{};
    if (url.value().query) {
      for (auto parameter :
           url.value().query.value() | ranges::views::split_when(is_separator) | ranges::views::transform(to_nvp)) {
        parameters.emplace_back(parameter);
      }
    }
    return parameters;
  }
  return tl::make_unexpected(url.error());
}

inline auto parse_query(
    std::string_view input) -> tl::expected<std::vector<query_parameter>, url_parse_errc> {
  [[maybe_unused]] bool validation_error = false;
  return parse_query(input, &validation_error);
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_CORE_PARSE_QUERY_HPP
