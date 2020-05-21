// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_RANGES_STRING_ELEMENT_RANGE_HPP
#define SKYR_V1_RANGES_STRING_ELEMENT_RANGE_HPP

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
  /// \param s
  string_element_iterator(
      std::basic_string_view<charT> s, std::basic_string_view<charT> separators)
      : view_(s)
      , separators_(separators)
      , separator_index_(std::min(view_.find_first_of(separators_), view_.size()))
      , check_last_(0)
  {
    if (!view_.empty()) {
      auto index = view_.substr(view_.size() - 1).find_first_of(separators_);
      check_last_ = (index != std::string_view::npos)? 1 : 0;
    }
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
    assert(!view_.empty() || (check_last_ != 0));
    return view_.substr(0, separator_index_);
  }

  ///
  /// \param other
  /// \return
  auto operator==(const string_element_iterator &other) const noexcept {
    return (view_.data() == other.view_.data()) && (check_last_ == other.check_last_);
  }

  ///
  /// \param other
  /// \return
  auto operator!=(const string_element_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    assert(!view_.empty() || (check_last_ != 0));
    if (view_.empty()) {
      --check_last_;
    } else {
      view_.remove_prefix(separator_index_);
      if (!view_.empty()) {
        view_.remove_prefix(1);
      }
      separator_index_ = std::min(view_.find_first_of(separators_), view_.size());
    }
  }

  value_type view_;
  value_type separators_;
  typename value_type::size_type separator_index_ = 0;
  int check_last_ = 0;

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
  /// \param s
  /// \param separators
  string_element_range(
      std::basic_string_view<charT> s,
      std::basic_string_view<charT> separators)
      : first_(s, separators), last_(s.substr(s.size()), separators) {}

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
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_RANGES_STRING_ELEMENT_RANGE_HPP
