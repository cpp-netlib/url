// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PATH_ITERATOR_HPP
#define SKYR_URL_PATH_ITERATOR_HPP

#include <string_view>
#include <optional>
#include <iterator>
#include <algorithm>

namespace skyr {
inline namespace v1 {
///
class path_element_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = std::string_view;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_pointer = std::add_pointer<const value_type>::type;
  ///
  using pointer = const_pointer;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  path_element_iterator() = default;

  ///
  /// \param path
  explicit path_element_iterator(std::string_view path)
      : it_(!path.empty() ? std::make_optional(std::begin(path)) : std::nullopt)
      , last_(!path.empty() ? std::make_optional(std::end(path)) : std::nullopt) {}

  ///
  /// \return
  auto &operator++() {
    increment();
    return *this;
  }

  ///
  /// \return
  auto operator++(int) {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  auto operator*() const noexcept -> const_reference {
    assert(it_);
    auto delimiter = std::find_if(
        it_.value(), last_.value(), [](auto c) { return (c == '/') || (c == '\\'); });
    return std::string_view(
        std::addressof(*it_.value()),
        std::distance(it_.value(), delimiter));
  }

  ///
  /// \param other
  /// \return
  auto operator==(const path_element_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  auto operator!=(const path_element_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    assert(it_);
    it_ = std::find_if(
        it_.value(), last_.value(), [](auto c) { return (c == '/') || (c == '\\'); });
    if (it_ == last_) {
      it_ = std::nullopt;
    } else {
      ++it_.value();
    }
  }

  std::optional<value_type::const_iterator> it_, last_;

};

///
class path_element_range {
 public:

  ///
  using const_iterator = path_element_iterator;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  path_element_range() = default;

  ///
  /// \param path
  explicit path_element_range(std::string_view path)
      : first_(path), last_() {}

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

  path_element_iterator first_, last_;

};
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_PATH_ITERATOR_HPP
