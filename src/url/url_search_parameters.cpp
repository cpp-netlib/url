// Copyright 2017-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <skyr/url/percent_encoding/percent_encode_range.hpp>
#include <skyr/url/url_search_parameters.hpp>
#include <skyr/url.hpp>

namespace skyr {
url_search_parameters::url_search_parameters(
    std::string_view query) {
  initialize(query);
}

url_search_parameters::url_search_parameters(
    url &url)
    : url_(&url) {
  if (url.record().query) {
    initialize(url.record().query.value());
  }
}

url_search_parameters::url_search_parameters(
    std::initializer_list<value_type> parameters)
  : parameters_(parameters) {}

void url_search_parameters::append(
    const string_type &name,
    const string_type &value) {
  parameters_.emplace_back(name, value);
  update();
}

namespace {
template <class Iterator>
inline auto remove_parameter(Iterator first, Iterator last, std::string_view name) {
  return std::remove_if(
      first, last,
      [&name] (const auto &parameter) { return name == parameter.first; });
}

template <class Iterator>
inline auto find_parameter(Iterator first, Iterator last, std::string_view name) {
  return std::find_if(
      first, last,
      [&name] (const auto &parameter) { return name == parameter.first; });
}
}  // namespace

void url_search_parameters::remove(
    const string_type &name) {
  auto first = std::begin(parameters_), last = std::end(parameters_);
  auto it = remove_parameter(first, last, name);
  parameters_.erase(it, last);
  update();
}

std::optional<url_search_parameters::string_type> url_search_parameters::get(
    const string_type &name) const {
  auto first = std::begin(parameters_), last = std::end(parameters_);
  auto it = find_parameter(first, last, name);
  return (it != last) ? std::make_optional(it->second) : std::nullopt;
}

std::vector<url_search_parameters::string_type> url_search_parameters::get_all(
    const string_type &name) const {
  std::vector<string_type> result;
  result.reserve(parameters_.size());
  for (auto [parameter_name, value] : parameters_) {
    if (parameter_name == name) {
      result.push_back(value);
    }
  }
  return result;
}

bool url_search_parameters::contains(const string_type &name) const noexcept {
  auto first = std::begin(parameters_), last = std::end(parameters_);
  return find_parameter(first, last, name) != last;
}

void url_search_parameters::set(
    const string_type &name,
    const string_type &value) {
  auto first = std::begin(parameters_), last = std::end(parameters_);
  auto it = find_parameter(first, last, name);
  if (it != last) {
    it->second = value;
    it = remove_parameter(++it, last, name);
    parameters_.erase(it, last);
  } else {
    parameters_.emplace_back(name, value);
  }
  update();
}

void url_search_parameters::clear() noexcept {
  parameters_.clear();
  update();
}

void url_search_parameters::sort() {
  auto first = std::begin(parameters_), last = std::end(parameters_);
  std::sort(
      first, last,
      [](const auto &lhs, const auto &rhs) -> bool { return lhs.first < rhs.first; });
  update();
}

url_search_parameters::string_type url_search_parameters::to_string() const {
  auto result = string_type{};

  auto first = std::begin(parameters_), last = std::end(parameters_);
  auto it = first;
  while (it != last) {
    result.append(it->first);
    result.append("=");
    result.append(it->second);

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

  for (auto [name, value] : search_parameter_range(query)) {
    parameters_.emplace_back(name, value);
  }
}

void url_search_parameters::update() {
//  if (url_) {
//    auto query = to_string();
//    parameters_.clear();
//    url_->set_search(query);
//  }
}
}  // namespace skyr
