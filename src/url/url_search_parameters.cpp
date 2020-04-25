// Copyright 2017-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/url_search_parameters.hpp>
#include <skyr/query/query_parameter_range.hpp>
#include <skyr/percent_encoding/percent_encode.hpp>
#include <skyr/percent_encoding/percent_decode.hpp>
#include <skyr/url.hpp>

namespace skyr {
inline namespace v1 {
url_search_parameters::url_search_parameters(
    url *url)
    : url_(url) {
  if (url_->record().query) {
    initialize(url_->record().query.value());
  }
}

void url_search_parameters::remove(
    std::string_view name) {
  static const auto equals_name = [&name](const auto &parameter) { return name == parameter.first; };

  auto first = std::begin(parameters_), last = std::end(parameters_);
  auto it = std::remove_if(first, last, equals_name);
  parameters_.erase(it, last);
  update();
}

auto url_search_parameters::get(
    std::string_view name) const -> std::optional<string_type> {
  static const auto equals_name = [&name](const auto &parameter) { return name == parameter.first; };

  auto first = std::begin(parameters_), last = std::end(parameters_);
  auto it = std::find_if(first, last, equals_name);
  return (it != last) ? std::make_optional(it->second) : std::nullopt;
}

auto url_search_parameters::get_all(
    std::string_view name) const -> std::vector<string_type> {
  std::vector<string_type> result;
  result.reserve(parameters_.size());
  for (auto[parameter_name, value] : parameters_) {
    if (parameter_name == name) {
      result.push_back(value);
    }
  }
  return result;
}

auto url_search_parameters::contains(std::string_view name) const noexcept -> bool {
  static const auto equals_name = [&name](const auto &parameter) { return name == parameter.first; };

  auto first = std::begin(parameters_), last = std::end(parameters_);
  return std::find_if(first, last, equals_name) != last;
}

void url_search_parameters::set(
    std::string_view name,
    std::string_view value) {
  static const auto equals_name = [&name](const auto &parameter) { return name == parameter.first; };

  auto first = std::begin(parameters_), last = std::end(parameters_);
  auto it = std::find_if(first, last, equals_name);
  if (it != last) {
    it->second = value;
    it = std::remove_if(++it, last, equals_name);
    parameters_.erase(it, last);
  } else {
    parameters_.emplace_back(name, value);
  }
  update();
}

auto url_search_parameters::to_string() const -> string_type {
  auto result = string_type{};

  auto first = std::begin(parameters_), last = std::end(parameters_);
  auto it = first;
  while (it != last) {
    result.append(percent_encode<string_type>(it->first));
    result.append("=");
    result.append(percent_encode<string_type>(it->second));

    ++it;
    if (it != last) {
      result.append("&");
    }
  }

  return result;
}

void url_search_parameters::initialize(std::string_view query) {
  if (!query.empty() && (query.front() == '?')) {
    query.remove_prefix(1);
  }

  for (auto [name, value] : query_parameter_range(query)) {
    auto name_ = percent_decode<std::string>(name).value_or(std::string(name));
    auto value_ = value? percent_decode<std::string>(value.value()).value_or(std::string(value.value())) : std::string();
    parameters_.emplace_back(name_, value_);
  }
}

void url_search_parameters::update() {
  if (url_) {
    auto query = to_string();
    parameters_.clear();
    url_->set_search(std::string_view(query));
  }
}
}  // namespace v1
}  // namespace skyr
