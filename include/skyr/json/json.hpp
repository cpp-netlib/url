// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_JSON_JSON_HPP
#define SKYR_JSON_JSON_HPP

#include <string>
#include <optional>
#include <vector>
#include <expected>
#include <nlohmann/json.hpp>
#include <format>
#include <skyr/core/parse_query.hpp>
#include <skyr/percent_encoding/percent_encode.hpp>
#include <skyr/percent_encoding/percent_decode.hpp>

namespace skyr {
namespace json {
///
enum class json_errc {
  ///
  invalid_query = 1,
};

inline auto encode_query(const nlohmann::json &json, char separator='&', char equal='=')
  -> std::expected<std::string, json_errc> {
  using namespace std::string_literals;

  std::string result{};

  if (!json.is_object()) {
    return std::unexpected(json_errc::invalid_query);
  }

  for (auto &[key, value] : json.items()) {
    if (value.is_string()) {
      result += std::format(
          "{}{}{}{}", percent_encode(key), equal, percent_encode(value.get<std::string>()), separator);
    } else if (value.is_array()) {
      for (auto &element : value.items()) {
        result += std::format(
            "{}{}{}{}", percent_encode(key), equal, percent_encode(element.value().get<std::string>()), separator);
      }
    } else {
      result += std::format("{}{}{}", percent_encode(key), equal, separator);
    }
  }

  return result.substr(0, result.size() - 1);
}

inline auto decode_query(std::string_view query) -> nlohmann::json {
  nlohmann::json object;

  auto parameters = parse_query(query);
  if (parameters) {
    for (auto &&[name, value] : parameters.value()) {
      const auto name_ = ::skyr::percent_decode(name).value();
      const auto value_ = value ? ::skyr::percent_decode(value.value()).value() : std::string();

      if (object.contains(name_)) {
        auto current_value = object[name_];
        if (current_value.is_string()) {
          auto prev_value = current_value.get<std::string>();
          object[name_] = std::vector<std::string>{prev_value, value_};
        } else if (current_value.is_array()) {
          auto values = current_value.get<std::vector<std::string>>();
          values.emplace_back(value_);
          object[name_] = values;
        }
      } else {
        object[name_] = value_;
      }
    }
  }
  return object;
}
}  // namespace json
}  // namespace skyr::v2

#endif //SKYR_JSON_JSON_HPP
