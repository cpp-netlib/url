// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_STRING_ELEMENT_RANGE_HPP
#define SKYR_URL_STRING_ELEMENT_RANGE_HPP

#include <string_view>
#include <optional>
#include <iterator>
#include <algorithm>

namespace skyr {
inline namespace v1 {
///
template <class charT, class Pred>
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
  explicit string_element_iterator(Pred pred)
      : it_(std::nullopt)
      , last_(std::nullopt)
      , pred_(pred)
  {}

  ///
  /// \param s
  string_element_iterator(std::basic_string_view<charT> s, Pred pred)
      : it_(!s.empty() ? std::make_optional(std::begin(s)) : std::nullopt)
      , last_(!s.empty() ? std::make_optional(std::end(s)) : std::nullopt)
      , pred_(pred) {}

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
    assert(it_);
    auto delimiter = std::find_if(it_.value(), last_.value(), pred_);
    return value_type(
        std::addressof(*it_.value()),
        std::distance(it_.value(), delimiter));
  }

  ///
  /// \param other
  /// \return
  auto operator==(const string_element_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  auto operator!=(const string_element_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    assert(it_);
    it_ = std::find_if(it_.value(), last_.value(), pred_);
    if (it_ == last_) {
      it_.reset();
    } else {
      ++it_.value();
    }
  }

  std::optional<typename value_type::const_iterator> it_, last_;
  Pred pred_;

};

///
template <class charT, class Pred>
class string_element_range {
 public:

  ///
  using const_iterator = string_element_iterator<charT, Pred>;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  string_element_range() = default;

  ///
  explicit string_element_range(Pred pred)
      : first_(pred), last_(pred)
  {}

  ///
  /// \param path
  string_element_range(std::basic_string_view<charT> s, Pred pred)
      : first_(s, pred), last_(pred) {}

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

  string_element_iterator<charT, Pred> first_, last_;

};

template <class charT, class Pred>
inline auto split(std::basic_string_view<charT> s, Pred pred) -> string_element_range<charT, Pred> {
  return string_element_range<charT, Pred>(s, pred);
}
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_STRING_ELEMENT_RANGE_HPP
