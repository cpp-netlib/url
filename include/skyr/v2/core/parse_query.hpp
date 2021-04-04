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

///
/// \param query
/// \param validation_error
/// \return
inline auto parse_query(
    std::string_view query, bool *validation_error) -> tl::expected<std::vector<query_parameter>, url_parse_errc> {
  if (!query.empty() && (query.front() == '?')) {
    query.remove_prefix(1);
  }

  auto url = details::basic_parse(query, validation_error, nullptr, nullptr, url_parse_state::query);
  if (url) {
    static constexpr auto is_separator = [](auto c) { return c == '&' || c == ';'; };

    static constexpr auto to_nvp = [](auto &&parameter) -> query_parameter {
      if (ranges::empty(parameter)) {
        return {};
      }

      auto view = std::string_view(std::addressof(*std::begin(parameter)), ranges::distance(parameter));
      auto delim = view.find_first_of('=');
      if (delim != std::string_view::npos) {
        return {std::string(view.substr(0, delim)), std::string(view.substr(delim + 1))};
      } else {
        return {std::string(view)};
      }
    };

    std::vector<query_parameter> parameters{};
    if (url.value().query) {
      for (auto &&parameter :
           url.value().query.value() | ranges::views::split_when(is_separator) | ranges::views::transform(to_nvp)) {
        parameters.emplace_back(parameter);
      }
    }
    return parameters;
  }
  return tl::make_unexpected(url.error());
}

///
/// \param query
/// \return
inline auto parse_query(
    std::string_view query) -> tl::expected<std::vector<query_parameter>, url_parse_errc> {
  [[maybe_unused]] bool validation_error = false;
  return parse_query(query, &validation_error);
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_CORE_PARSE_QUERY_HPP
