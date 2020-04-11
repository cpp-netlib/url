// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_QUERY_ITERATOR_HPP
#define SKYR_URL_QUERY_ITERATOR_HPP

namespace skyr {
inline namespace v1 {
///
class query_element_iterator {
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
  query_element_iterator() = default;

  ///
  /// \param query
  explicit query_element_iterator(std::string_view query, char separator='&')
      : it_(!query.empty() ? std::make_optional(std::begin(query)) : std::nullopt)
      , last_(!query.empty() ? std::make_optional(std::end(query)) : std::nullopt)
      , separator_(separator) {}

  ///
  /// \return
  query_element_iterator &operator++() {
    increment();
    return *this;
  }

  ///
  /// \return
  query_element_iterator operator++(int) {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  const_reference operator*() const noexcept {
    assert(it_);
    auto delimiter = std::find_if(
        it_.value(), last_.value(), [this](auto c) { return c == separator_; });
    return std::string_view(
        std::addressof(*it_.value()),
        std::distance(it_.value(), delimiter));
  }

  ///
  /// \param other
  /// \return
  bool operator==(const query_element_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  bool operator!=(const query_element_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    assert(it_);
    it_ = std::find_if(
        it_.value(), last_.value(), [this](auto c) { return c == separator_; });
    if (it_ == last_) {
      it_ = std::nullopt;
    } else {
      ++it_.value();
    }
  }

  std::optional<value_type::const_iterator> it_, last_;
  const char separator_ = '&';

};

///
class query_parameter_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = std::pair<std::string_view, std::string_view>;
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
  query_parameter_iterator() = default;

  ///
  /// \param query
  explicit query_parameter_iterator(std::string_view query, char separator='&', char equal='=')
      : it_(query, separator)
      , equal_(equal) {}

  ///
  /// \return
  query_parameter_iterator &operator++() {
    ++it_;
    return *this;
  }

  ///
  /// \return
  query_parameter_iterator operator++(int) {
    auto result = *this;
    ++it_;
    return result;
  }

  ///
  /// \return
  const_reference operator*() const noexcept {
    auto first = std::begin(*it_), last = std::end(*it_);

    auto equal_it = std::find_if(first, last, [this](auto c) { return (c == equal_); });

    auto name =
        std::string_view(std::addressof(*first), std::distance(first, equal_it));
    if (equal_it != last) {
      ++equal_it;
    }
    auto value = (equal_it == last) ?
                 std::string_view() :
                 std::string_view(std::addressof(*equal_it), std::distance(equal_it, last));

    return {name, value};
  }

  ///
  /// \param other
  /// \return
  bool operator==(const query_parameter_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  bool operator!=(const query_parameter_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  query_element_iterator it_;
  const char equal_ = '=';

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
  query_parameter_range() = default;

  ///
  /// \param query
  explicit query_parameter_range(std::string_view query, char separator='&')
      : first_(query, separator), last_() {}

  ///
  /// \return
  [[nodiscard]] const_iterator begin() const noexcept {
    return first_;
  }

  ///
  /// \return
  [[nodiscard]] const_iterator end() const noexcept {
    return last_;
  }

  ///
  /// \return
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return begin();
  }

  ///
  /// \return
  [[nodiscard]] const_iterator cend() const noexcept {
    return end();
  }

  ///
  /// \return
  [[nodiscard]] bool empty() const noexcept {
    return first_ == last_;
  }

  ///
  /// \return
  [[nodiscard]] size_type size() const noexcept {
    return static_cast<size_type>(std::distance(first_, last_));
  }

 private:

  query_parameter_iterator first_, last_;

};
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_QUERY_ITERATOR_HPP
