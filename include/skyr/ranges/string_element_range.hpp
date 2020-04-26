// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_STRING_ELEMENT_RANGE_HPP
#define SKYR_URL_STRING_ELEMENT_RANGE_HPP

#include <string_view>
#include <iterator>
#include <algorithm>
#include <cassert>

namespace skyr {
inline namespace v1 {
///
template <class charT>
class string_element_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = std::basic_string_view<charT>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_pointer = typename std::add_pointer<const charT>::type;
  ///
  using pointer = const_pointer;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  explicit string_element_iterator() = default;

  ///
  /// \param s
  string_element_iterator(
      std::basic_string_view<charT> s, std::basic_string_view<charT> separators)
      : view_(s)
      , separators_(separators)
      , separator_index_(std::min(view_.find_first_of(separators_), view_.size()))
  {
  }

  ///
  /// \return
  auto operator++() -> string_element_iterator & {
    increment();
    return *this;
  }

  ///
  /// \return
  auto operator++(int) -> value_type {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  auto operator*() const noexcept -> const_reference {
    assert(!view_.empty());
    return view_.substr(0, separator_index_);
  }

  ///
  /// \param other
  /// \return
  auto operator==(const string_element_iterator &other) const noexcept {
    return view_ == other.view_;
  }

  ///
  /// \param other
  /// \return
  auto operator!=(const string_element_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    assert(!view_.empty());
    view_.remove_prefix(separator_index_);
    if (!view_.empty()) {
      view_.remove_prefix(1);
    }
    separator_index_ = std::min(view_.find_first_of(separators_), view_.size());
  }

  value_type view_;
  value_type separators_;
  typename value_type::size_type separator_index_ = 0;

};

///
template <class charT>
class string_element_range {
 public:

  ///
  using const_iterator = string_element_iterator<charT>;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  string_element_range() = default;

  ///
  /// \param s
  /// \param delimiters
  string_element_range(
      string_element_iterator<charT> first,
      string_element_iterator<charT> last)
      : first_(first), last_(last) {}

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

  string_element_iterator<charT> first_, last_;

};

///
/// \tparam charT
/// \param s
/// \param separators
/// \returns A lazy range of string_views, delimited by any character in `separators`
template <class charT>
inline auto split(std::basic_string_view<charT> s, decltype(s) separators)
    -> string_element_range<charT> {
  return { string_element_iterator<charT>(s, separators), string_element_iterator<charT>() };
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_STRING_ELEMENT_RANGE_HPP
