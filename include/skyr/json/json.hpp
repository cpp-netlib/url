// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_JSON_HPP
#define SKYR_URL_JSON_HPP

#include <string>
#include <skyr/query/query_iterator.hpp>
#include <nlohmann/json.hpp>

namespace skyr {
inline namespace v1 {
namespace json {
struct decode_options {

};

inline std::string encode(const nlohmann::json json, char separator = '&', char equal = '=') {
  return "";
}

nlohmann::json decode(std::string_view query, char separator = '&', char equal = '=') {
  if (query[0] == '?') {
    query.remove_prefix(0);
  }

  nlohmann::json object;
  for (auto [key, value] : query_parameter_range(query)) {
    if (object.end() == object.find(key)) {
      object[key] = value;
    }
    else {
      if (object[key].is_object()) {
        //  object[key] = { object.get(key), value };
      }
      else {
        // object.arr
      }
    }
    object[key] = value;
  }
  return object;
}
}  // namespace json
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_QUERYSTRING_HPP
