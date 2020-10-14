// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_JSON_JSON_HPP
#define SKYR_V1_JSON_JSON_HPP

#include <string>
#include <optional>
#include <vector>
#include <tl/expected.hpp>
#include <range/v3/view/split_when.hpp>
#include <range/v3/view/transform.hpp>
#include <nlohmann/json.hpp>
#include <skyr/v1/percent_encoding/percent_encode.hpp>
#include <skyr/v1/percent_encoding/percent_decode.hpp>

namespace skyr {
inline namespace v1 {
namespace json {
///
enum class json_errc {
  ///
  invalid_query = 1,
};

inline auto encode_query(const nlohmann::json &json, char separator='&', char equal='=')
  -> tl::expected<std::string, json_errc> {
  using namespace std::string_literals;

  auto result = ""s;

  if (!json.is_object()) {
    return tl::make_unexpected(json_errc::invalid_query);
  }

  for (auto &[key, value] : json.items()) {

    if (value.is_string()) {
      result += percent_encode(key);
      result += equal;
      result += percent_encode(value.get<std::string>());
      result += separator;
    }
    else if (value.is_array()) {
      for (auto &element : value.items()) {
        result += percent_encode(key);
        result += equal;
        result += percent_encode(element.value().get<std::string>());
        result += separator;
      }
    }
    else {
      result += percent_encode(key);
      result += equal;
      result += separator;
    }
  }

  return result.substr(0, result.size() - 1);
}

inline auto decode_query(std::string_view query, char separator='&', char equal='=') -> nlohmann::json {
  if (query[0] == '?') {
    query.remove_prefix(1);
  }

  static constexpr auto is_separator = [] (auto &&c) {
    return c == '&' || c == ';';
  };

  static constexpr auto to_nvp = [] (auto &&param) -> std::pair<std::string_view, std::optional<std::string_view>> {
    auto element = std::string_view(std::addressof(*std::begin(param)), ranges::distance(param));
    auto delim = element.find_first_of("=");
    if (delim != std::string_view::npos) {
      return { element.substr(0, delim), element.substr(delim + 1) };
    }
    else {
      return { element, std::nullopt };
    }
  };

  nlohmann::json object;
  for (auto [name, value] : query | ranges::views::split_when(is_separator) | ranges::views::transform(to_nvp)) {
    const auto name_ = skyr::percent_decode(name).value();
    const auto value_ = value? skyr::percent_decode(value.value()).value() : std::string();

    if (object.contains(name_)) {
      auto current_value = object[name_];
      if (current_value.is_string()) {
        auto prev_value = current_value.get<std::string>();
        object[name_] = std::vector<std::string>{prev_value, value_};
      }
      else if (current_value.is_array()) {
        auto values = current_value.get<std::vector<std::string>>();
        values.emplace_back(value_);
        object[name_] = values;
      }
    }
    else {
      object[name_] = value_;
    }
  }
  return object;
}
}  // namespace json
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_JSON_JSON_HPP
