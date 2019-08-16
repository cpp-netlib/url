// Copyright 2017-18 Glyn Matthews.
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
#include <skyr/url/url_record.hpp>

namespace skyr {
/// Supports iterating through
/// [URL search parameters](https://url.spec.whatwg.org/#urlsearchparams)
///
/// The API closely follows the
/// [WhatWG IDL specification](https://url.spec.whatwg.org/#interface-urlsearchparams)
class url_search_parameters {

  friend class url;

 public:

  /// ASCII string type
  /// \sa url::string_type
  using string_type = std::string;

  /// A key-value pair
  using value_type = std::pair<string_type, string_type>;

  /// An iterator through the search parameters
  using const_iterator = std::vector<value_type>::const_iterator;

  /// Constructor
  url_search_parameters();

  /// Constructor
  ///
  /// \param query The search string
  explicit url_search_parameters(std::string_view query);

  /// Constructor
  ///
  /// \param url The URL record
  explicit url_search_parameters(url_record &url);

  /// Appends a name-value pair to the search string
  ///
  /// \param name The parameter name
  /// \param value The parameter value
  void append(const string_type &name, const string_type &value);

  /// Removes a parameter from the search string
  ///
  /// \param name The name of the parameter to remove
  void remove(const string_type &name);

  /// \param name
  /// \returns The first search parameter value with the given name
  std::optional<string_type> get(const string_type &name) const noexcept;

  /// \param name
  /// \returns All search parameter values with the given name
  std::vector<string_type> get_all(const string_type &name) const;

  /// Tests if there is a parameter with the given name
  ///
  /// \param name The search parameter name
  /// \returns `true` if the value is in the search parameters,
  /// `false` otherwise.
  bool contains(const string_type &name) const noexcept;

  /// Sets a URL search parameter
  ///
  /// \param name
  /// \param value
  void set(const string_type &name, const string_type &value);

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

  /// \returns `true` if the URL search string is empty, `false`
  ///          otherwise
  bool empty() const noexcept {
    return parameters_.empty();
  }

  /// \returns An iterator to the first element in the search parameters
  const_iterator begin() const noexcept {
    return parameters_.begin();
  }

  /// \returns An iterator to the last element in the search parameters
  const_iterator end() const noexcept {
    return parameters_.end();
  }

  /// \returns The serialized URL search parameters
  string_type to_string() const;

 private:
  void initialize(std::string_view query);
  void update();

  std::vector<value_type> parameters_;
  std::optional<std::reference_wrapper<url_record>> url_;
};
}  // namespace skyr

#endif  // SKYR_URL_SEARCH_PARAMETERS_INC
