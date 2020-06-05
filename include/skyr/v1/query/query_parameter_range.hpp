// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_QUERY_QUERY_PARAMETER_RANGE_HPP
#define SKYR_V1_QUERY_QUERY_PARAMETER_RANGE_HPP

#include <string_view>
#include <optional>
#include <iterator>
#include <type_traits>
#include <algorithm>
#include <utility>
#include <skyr/v1/ranges/string_element_range.hpp>

namespace skyr {
inline namespace v1 {
///
class query_parameter_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = std::pair<std::string_view, std::optional<std::string_view>>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_pointer = const value_type *;
  ///
  using pointer = const_pointer;
  ///
  using difference_type = std::ptrdiff_t;
  ///
  using size_type = std::size_t;

  ///
  /// \param query
  explicit query_parameter_iterator(std::string_view query)
      : it_(query, std::string_view("&;")) {}

  ///
  /// \return
  auto operator++() -> query_parameter_iterator & {
    ++it_;
    return *this;
  }

  ///
  /// \return
  auto operator++(int) {
    auto result = *this;
    ++it_;
    return result;
  }

  ///
  /// \return
  auto operator*() const noexcept -> const_reference {
    auto element = *it_;
    auto delim = element.find_first_of("=");
    if (delim != value_type::first_type::npos) {
      return { element.substr(0, delim), element.substr(delim + 1) };
    }
    else {
      return { element, std::nullopt };
    }
  }

  ///
  /// \param other
  /// \return
  auto operator==(const query_parameter_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  auto operator!=(const query_parameter_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  string_element_iterator<char> it_;

};

///
class query_parameter_range {
 public:

  ///
  using const_iterator = query_parameter_iterator;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  /// \param query
  explicit query_parameter_range(std::string_view query)
      : first_(query), last_(query.substr(query.size())) {}

  ///
  /// \return
  [[nodiscard]] auto begin() const noexcept {
    return first_;
  }

  ///
  /// \return
  [[nodiscard]] auto end() const noexcept {
    return last_;
  }

  ///
  /// \return
  [[nodiscard]] auto cbegin() const noexcept {
    return begin();
  }

  ///
  /// \return
  [[nodiscard]] auto cend() const noexcept {
    return end();
  }

  ///
  /// \return
  [[nodiscard]] auto empty() const noexcept {
    return first_ == last_;
  }

  ///
  /// \return
  [[nodiscard]] auto size() const noexcept {
    return static_cast<size_type>(std::distance(first_, last_));
  }

 private:

  query_parameter_iterator first_, last_;

};
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_QUERY_QUERY_PARAMETER_RANGE_HPP
