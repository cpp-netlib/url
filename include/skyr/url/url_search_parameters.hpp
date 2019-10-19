// Copyright 2017-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_SEARCH_PARAMETERS_INC
#define SKYR_URL_SEARCH_PARAMETERS_INC

#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <optional>
#include <algorithm>
#include <cassert>

namespace skyr {
inline namespace v1 {
///
class search_element_iterator {
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
  search_element_iterator() = default;

  ///
  /// \param query
  explicit search_element_iterator(std::string_view query)
      : it_(!query.empty() ? std::make_optional(std::begin(query)) : std::nullopt), last_(std::end(query)) {}

  ///
  /// \return
  search_element_iterator &operator++() {
    increment();
    return *this;
  }

  ///
  /// \return
  search_element_iterator operator++(int) {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  const_reference operator*() const noexcept {
    assert(it_);
    auto next = std::find_if(
        it_.value(), last_.value(), [](auto c) { return (c == '&') || (c == ';'); });
    return std::string_view(
        std::addressof(*it_.value()),
        std::distance(it_.value(), next));
  }

  ///
  /// \param other
  /// \return
  bool operator==(const search_element_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  bool operator!=(const search_element_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    assert(it_);
    it_ = std::find_if(
        it_.value(), last_.value(), [](auto c) { return (c == '&') || (c == ';'); });
    if (it_ == last_) {
      it_ = std::nullopt;
    } else {
      ++it_.value();
    }
  }

  std::optional<std::string_view::const_iterator> it_, last_;

};

///
class search_parameter_iterator {
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
  search_parameter_iterator() = default;

  ///
  /// \param query
  explicit search_parameter_iterator(std::string_view query)
      : it_(query) {}

  ///
  /// \return
  search_parameter_iterator &operator++() {
    increment();
    return *this;
  }

  ///
  /// \return
  search_parameter_iterator operator++(int) {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  const_reference operator*() const noexcept {
    auto first = std::begin(*it_), last = std::end(*it_);

    auto equal = std::find_if(first, last, [](auto c) { return (c == '='); });

    auto name =
        std::string_view(std::addressof(*first), std::distance(first, equal));
    if (equal != last) {
      ++equal;
    }
    auto value =
        std::string_view(std::addressof(*equal), std::distance(equal, last));

    return {name, value};
  }

  ///
  /// \param other
  /// \return
  bool operator==(const search_parameter_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  bool operator!=(const search_parameter_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    ++it_;
  }

  search_element_iterator it_;

};

///
class search_parameter_range {
 public:

  ///
  using const_iterator = search_parameter_iterator;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  search_parameter_range() = default;

  ///
  /// \param query
  explicit search_parameter_range(std::string_view query)
      : first_(query), last_() {}

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

  search_parameter_iterator first_, last_;

};

class url;

/// Supports iterating through
/// [URL search parameters](https://url.spec.whatwg.org/#urlsearchparams)
///
/// The API closely follows the
/// [WhatWG IDL specification](https://url.spec.whatwg.org/#interface-urlsearchparams)
class url_search_parameters {

  friend class url;

 public:

  /// string type
  /// \sa url::string_type
  using string_type = std::string;

  /// A key-value pair
  using value_type = std::pair<string_type, string_type>;

  /// An iterator through the search parameters
  using const_iterator = std::vector<value_type>::const_iterator;

  /// An alias to \c const_iterator
  using iterator = const_iterator;

  /// \c std::size_t
  using size_type = std::size_t;

  /// Default constructor
  url_search_parameters() = default;

  /// Constructor
  /// \param query The search string
  explicit url_search_parameters(std::string_view query);

  /// Constructor
  /// \param url The URL
  explicit url_search_parameters(url *url);

  ///
  /// \param parameters
  url_search_parameters(std::initializer_list<value_type> parameters);

  ///
  /// \param other
  void swap(url_search_parameters &other) noexcept;

  /// Appends a name-value pair to the search string
  ///
  /// \param name The parameter name
  /// \param value The parameter value
  void append(std::string_view name, std::string_view value);

  /// Removes a parameter from the search string
  ///
  /// \param name The name of the parameter to remove
  void remove(std::string_view name);

  /// \param name The search parameter name
  /// \returns The first search parameter value with the given name
  [[nodiscard]] std::optional<string_type> get(std::string_view name) const;

  /// \param name The search parameter name
  /// \returns All search parameter values with the given name
  [[nodiscard]] std::vector<string_type> get_all(std::string_view name) const;

  /// Tests if there is a parameter with the given name
  ///
  /// \param name The search parameter name
  /// \returns `true` if the value is in the search parameters,
  /// `false` otherwise.
  [[nodiscard]] bool contains(std::string_view name) const noexcept;

  /// Sets a URL search parameter
  ///
  /// \param name The search parameter name
  /// \param value The search parameter value
  void set(std::string_view name, std::string_view value);

  /// Clears the search parameters
  ///
  /// \post `empty() == true`
  void clear() noexcept;

  /// Sorts the search parameters alphanumerically
  ///
  /// https://url.spec.whatwg.org/#example-searchparams-sort
  ///
  /// ```
  /// auto url = skyr::url(
  ///   "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
  /// url.search_parameters().sort();
  /// assert(url.search() == "?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88");
  /// ```
  void sort();

  /// \returns An iterator to the first element in the search parameters
  [[nodiscard]] const_iterator begin() const noexcept {
    return parameters_.begin();
  }

  /// \returns An iterator to the last element in the search parameters
  [[nodiscard]] const_iterator end() const noexcept {
    return parameters_.end();
  }

  /// \returns An iterator to the first element in the search parameters
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return begin();
  }

  /// \returns An iterator to the last element in the search parameters
  [[nodiscard]] const_iterator cend() const noexcept {
    return end();
  }

  /// \returns `true` if the URL search string is empty, `false`
  ///          otherwise
  [[nodiscard]] bool empty() const noexcept {
    return parameters_.empty();
  }

  /// \returns The size of the parameters array (i.e. the
  ///          number of parameters)
  [[nodiscard]] size_type size() const noexcept {
    return parameters_.size();
  }

  /// \returns The serialized URL search parameters
  [[nodiscard]] string_type to_string() const;

 private:
  void initialize(std::string_view query);
  void update();

  std::vector<value_type> parameters_;
  url *url_ = nullptr;
};

/// 
/// \param lhs
/// \param rhs
inline void swap(url_search_parameters &lhs, url_search_parameters &rhs) noexcept {
  lhs.swap(rhs);
}
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_URL_SEARCH_PARAMETERS_INC
