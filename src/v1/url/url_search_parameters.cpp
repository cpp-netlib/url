// Copyright 2017-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <range/v3/view/split_when.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/action/erase.hpp>
#include <skyr/v1/url_search_parameters.hpp>
#include <skyr/v1/percent_encoding/percent_encode.hpp>
#include <skyr/v1/percent_encoding/percent_decode.hpp>
#include <skyr/v1/url.hpp>

namespace skyr {
inline namespace v1 {
namespace {
struct is_name {
  explicit is_name(std::string_view name)
      : name_(name)
  {}

  auto operator () (const std::pair<std::string, std::string> &parameter) noexcept {
    return name_ == parameter.first;
  }

  std::string_view name_;
};
}  // namespace

url_search_parameters::url_search_parameters(
    url *url)
    : url_(url) {
  if (url_->record().query) {
    initialize(url_->record().query.value());
  }
}

void url_search_parameters::remove(
    std::string_view name) {
  auto it = ranges::remove_if(parameters_, is_name(name));
  ranges::erase(parameters_, it, ranges::end(parameters_));
  update();
}

auto url_search_parameters::get(
    std::string_view name) const -> std::optional<string_type> {
  auto it = ranges::find_if(parameters_, is_name(name));
  return (it != ranges::cend(parameters_)) ? std::make_optional(it->second) : std::nullopt;
}

auto url_search_parameters::get_all(
    std::string_view name) const -> std::vector<string_type> {
  std::vector<string_type> result;
  result.reserve(parameters_.size());
  for (auto[parameter_name, value] : parameters_) {
    if (parameter_name == name) {
      result.emplace_back(value);
    }
  }
  return result;
}

auto url_search_parameters::contains(std::string_view name) const noexcept -> bool {
  return ranges::find_if(parameters_, is_name(name)) != ranges::cend(parameters_);
}

void url_search_parameters::set(
    std::string_view name,
    std::string_view value) {
  auto it = ranges::find_if(parameters_, is_name(name));
  if (it != ranges::end(parameters_)) {
    it->second = value;

    ++it;
    it = std::remove_if(it, ranges::end(parameters_), is_name(name));
    ranges::erase(parameters_, it, ranges::end(parameters_));
  } else {
    parameters_.emplace_back(name, value);
  }
  update();
}

auto url_search_parameters::to_string() const -> string_type {
  auto result = string_type{};

  bool start = true;
  for (const auto &[name, value] : parameters_) {
    if (start) {
      start = false;
    }
    else {
      result.append("&");
    }
    result.append(percent_encode(name));
    result.append("=");
    result.append(percent_encode(value));
  }

  return result;
}

void url_search_parameters::initialize(std::string_view query) {
  if (!query.empty() && (query.front() == '?')) {
    query.remove_prefix(1);
  }

  static constexpr auto is_separator = [] (auto &&c) {
    return c == '&' || c == ';';
  };

  static constexpr auto to_nvp = [] (auto &&param) -> std::pair<std::string_view, std::optional<std::string_view>> {
    auto element = std::string_view(std::addressof(*ranges::begin(param)), ranges::distance(param));
    auto delim = element.find_first_of("=");
    if (delim != std::string_view::npos) {
      return { element.substr(0, delim), element.substr(delim + 1) };
    }
    else {
      return { element, std::nullopt };
    }
  };

  for (auto [name, value] : query | ranges::views::split_when(is_separator) | ranges::views::transform(to_nvp)) {
    auto name_ = percent_decode(name).value_or(std::string(name));
    auto value_ = value? percent_decode(value.value()).value_or(std::string(value.value())) : std::string();
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
