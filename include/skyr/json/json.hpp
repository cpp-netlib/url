// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_JSON_HPP
#define SKYR_URL_JSON_HPP

#include <string>
#include <system_error>
#include <vector>
#include <tl/expected.hpp>
#include <nlohmann/json.hpp>
#include <skyr/query/query_iterator.hpp>
#include <skyr/percent_encoding/percent_encode.hpp>
#include <skyr/percent_encoding/percent_decode.hpp>

namespace skyr {
inline namespace v1 {
namespace json {
inline auto encode_query(const nlohmann::json &json, char separator='&', char equal='=')
  -> tl::expected<std::string, std::error_code> {
  using namespace std::string_literals;

  auto result = ""s;
  for (auto &[key, value] : json.items()) {

    if (value.is_string()) {
      result += percent_encode<std::string>(key);
      result += equal;
      result += percent_encode<std::string>(value.get<std::string>());
      result += separator;
    }
    else if (value.is_array()) {
      for (auto &element : value.items()) {
        result += percent_encode<std::string>(key);
        result += equal;
        result += percent_encode<std::string>(element.value().get<std::string>());
        result += separator;
      }
    }
    else {
      result += percent_encode<std::string>(key);
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

  nlohmann::json object;
  for (auto [key, value] : query_parameter_range(query)) {
    const auto key_ = skyr::percent_decode<std::string>(key).value();
    const auto value_ = value? skyr::percent_decode<std::string>(value.value()).value() : std::string();

    if (object.contains(key_)) {
      auto current_value = object[key_];
      if (current_value.is_string()) {
        auto prev_value = current_value.get<std::string>();
        object[key_] = std::vector<std::string>{prev_value, value_};
      }
      else if (current_value.is_array()) {
        auto values = current_value.get<std::vector<std::string>>();
        values.emplace_back(value_);
        object[key_] = values;
      }
    }
    else {
      object[key_] = value_;
    }
  }
  return object;
}
}  // namespace json
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_QUERYSTRING_HPP
