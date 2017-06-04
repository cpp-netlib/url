// Copyright 2012-2017 Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <locale>
#include <algorithm>
#include <functional>
#include <vector>
#include "network/uri/whatwg/url.hpp"
#include "detail/uri_parse.hpp"
#include "detail/uri_advance_parts.hpp"
#include "detail/uri_percent_encode.hpp"
#include "detail/uri_resolve.hpp"
#include "detail/algorithm.hpp"

namespace network {
namespace whatwg {
namespace {
const std::vector<std::pair<url::string_type, optional<std::uint16_t>>> &special_schemes() {
  const static std::vector<std::pair<url::string_type, optional<std::uint16_t>>> schemes = {
    {"ftp", 21},
    {"file", nullopt},
    {"gopher", 70},
    {"http", 80},
    {"https", 443},
    {"ws", 80},
    {"wss", 443},
  };
  return schemes;
}
}  // namespace

url::query_iterator::query_iterator() : query_{}, kvp_{} {}

url::query_iterator::query_iterator(optional<::network::detail::uri_part> query)
  : query_(query)
  , kvp_{} {
  if (query_ && query_->empty()) {
    query_ = nullopt;
  }
  else {
    assign_kvp();
  }
}

url::query_iterator::query_iterator(const query_iterator &other)
  : query_(other.query_)
  , kvp_(other.kvp_) {

}

url::query_iterator &url::query_iterator::operator = (const query_iterator &other) {
  auto tmp(other);
  swap(tmp);
  return *this;
}

url::query_iterator::reference url::query_iterator::operator ++ () noexcept {
  increment();
  return kvp_;
}

url::query_iterator::value_type url::query_iterator::operator ++ (int) noexcept {
  auto original = kvp_;
  increment();
  return original;
}

url::query_iterator::reference url::query_iterator::operator * () const noexcept {
  return kvp_;
}

url::query_iterator::pointer url::query_iterator::operator -> () const noexcept {
  return std::addressof(kvp_);
}

bool url::query_iterator::operator==(const query_iterator &other) const noexcept {
  if (!query_ && !other.query_) {
    return true;
  }
  else if (query_ && other.query_) {
    // since we're comparing substrings, the address of the first
    // element in each iterator must be the same
    return std::addressof(kvp_.first) == std::addressof(other.kvp_.first);
  }
  return false;
}

void url::query_iterator::swap(query_iterator &other) noexcept {
  std::swap(query_, other.query_);
  std::swap(kvp_, other.kvp_);
}

void url::query_iterator::advance_to_next_kvp() noexcept {
  auto first = std::begin(*query_), last = std::end(*query_);

  auto sep_it = std::find_if(
      first, last, [](char c) -> bool { return c == '&' || c == ';'; });

  if (sep_it != last) {
    ++sep_it; // skip next separator
  }

  // reassign query to the next element
  query_ = ::network::detail::uri_part(sep_it, last);
}

void url::query_iterator::assign_kvp() noexcept {
  auto first = std::begin(*query_), last = std::end(*query_);

  auto sep_it = std::find_if(
      first, last, [](char c) -> bool { return c == '&' || c == ';'; });
  auto eq_it =
      std::find_if(first, sep_it, [](char c) -> bool { return c == '='; });

  kvp_.first = string_view(std::addressof(*first), std::distance(first, eq_it));
  if (eq_it != sep_it) {
    ++eq_it; // skip '=' symbol
  }
  kvp_.second = string_view(std::addressof(*eq_it), std::distance(eq_it, sep_it));
}

void url::query_iterator::increment() noexcept {
  assert(query_);

  if (!query_->empty()) {
    advance_to_next_kvp();
    assign_kvp();
  }

  if (query_->empty()) {
    query_ = nullopt;
  }
}

url::path_iterator::path_iterator() : path_{}, element_{} {}

url::path_iterator::path_iterator(optional<::network::detail::uri_part> query)
  : path_(query)
  , element_{} {
  if (path_ && path_->empty()) {
    path_ = nullopt;
  }
  else {
    // skip past '/'
    advance_to_next_element();
    assign_element();
  }
}

url::path_iterator::path_iterator(const path_iterator &other)
  : path_(other.path_)
  , element_(other.element_) {

}

url::path_iterator &url::path_iterator::operator = (const path_iterator &other) {
  auto tmp(other);
  swap(tmp);
  return *this;
}

url::path_iterator::reference url::path_iterator::operator ++ () noexcept {
  increment();
  return element_;
}

url::path_iterator::value_type url::path_iterator::operator ++ (int) noexcept {
  auto original = element_;
  increment();
  return original;
}

url::path_iterator::reference url::path_iterator::operator * () const noexcept {
  return element_;
}

url::path_iterator::pointer url::path_iterator::operator -> () const noexcept {
  return std::addressof(element_);
}

bool url::path_iterator::operator==(const path_iterator &other) const noexcept {
  if (!path_ && !other.path_) {
    return true;
  }
  else if (path_ && other.path_) {
    // since we're comparing substrings, the address of the first
    // element in each iterator must be the same
    return std::addressof(element_) == std::addressof(other.element_);
  }
  return false;
}

void url::path_iterator::swap(path_iterator &other) noexcept {
  std::swap(path_, other.path_);
  std::swap(element_, other.element_);
}

void url::path_iterator::advance_to_next_element() noexcept {
  auto first = std::begin(*path_), last = std::end(*path_);

  auto sep_it = std::find_if(
      first, last, [](char c) -> bool { return c == '/'; });

  if (sep_it != last) {
    ++sep_it; // skip next separator
  }

  // reassign query to the next element
  path_ = ::network::detail::uri_part(sep_it, last);
}

void url::path_iterator::assign_element() noexcept {
  auto first = std::begin(*path_), last = std::end(*path_);

  auto sep_it = std::find_if(
      first, last, [](char c) -> bool { return c == '/'; });

  element_ = string_view(std::addressof(*first), std::distance(first, sep_it));
}

void url::path_iterator::increment() noexcept {
  assert(path_);

  if (!path_->empty()) {
    advance_to_next_element();
    assign_element();
  }

  if (path_->empty()) {
    path_ = nullopt;
  }
}

url::url() : url_view_(url_), url_parts_(), cannot_be_a_base_url_(false) {}

url::url(const url &other)
    : url_(other.url_),
      url_view_(url_),
      url_parts_(),
      cannot_be_a_base_url_(other.cannot_be_a_base_url_) {
  ::network::detail::advance_parts(url_view_, url_parts_, other.url_parts_);
}

url::url(url &&other) noexcept
    : url_(std::move(other.url_)),
      url_view_(url_),
      url_parts_(std::move(other.url_parts_)),
      cannot_be_a_base_url_(std::move(other.cannot_be_a_base_url_)) {
  ::network::detail::advance_parts(url_view_, url_parts_, other.url_parts_);
  other.url_.clear();
  other.url_view_ = string_view(other.url_);
  other.url_parts_ = ::network::detail::uri_parts();
}

url::~url() {}

url &url::operator=(url other) {
  other.swap(*this);
  return *this;
}

void url::swap(url &other) noexcept {
  auto parts = url_parts_;
  advance_parts(other.url_view_, url_parts_, other.url_parts_);
  url_.swap(other.url_);
  url_view_.swap(other.url_view_);
  advance_parts(other.url_view_, other.url_parts_, parts);
  std::swap(cannot_be_a_base_url_, other.cannot_be_a_base_url_);
}

url::const_iterator url::begin() const noexcept { return url_view_.begin(); }

url::const_iterator url::end() const noexcept { return url_view_.end(); }

bool url::has_scheme() const noexcept {
  return static_cast<bool>(url_parts_.scheme);
}

url::string_view url::scheme() const noexcept {
  return has_scheme() ? static_cast<string_view>(*url_parts_.scheme)
                      : string_view{};
}

bool url::has_user_info() const noexcept {
  return static_cast<bool>(url_parts_.user_info);
}

url::string_view url::user_info() const noexcept {
  return has_user_info() ? static_cast<string_view>(*url_parts_.user_info)
                         : string_view{};
}

bool url::has_host() const noexcept {
  return static_cast<bool>(url_parts_.host);
}

url::string_view url::host() const noexcept {
  return has_host() ? static_cast<string_view>(*url_parts_.host)
                    : string_view{};
}

bool url::has_port() const noexcept {
  return static_cast<bool>(url_parts_.port);
}

url::string_view url::port() const noexcept {
  return has_port() ? static_cast<string_view>(*url_parts_.port)
                    : string_view{};
}

bool url::has_path() const noexcept {
  return static_cast<bool>(url_parts_.path);
}

url::string_view url::path() const noexcept {
  return has_path() ? static_cast<string_view>(*url_parts_.path)
                    : string_view{};
}

url::path_iterator url::path_begin() const noexcept {
  return has_path()? url::path_iterator{url_parts_.path} : url::path_iterator{};
}

url::path_iterator url::path_end() const noexcept {
  return url::path_iterator{};
}

bool url::has_query() const noexcept {
  return static_cast<bool>(url_parts_.query);
}

url::string_view url::query() const noexcept {
  return has_query() ? static_cast<string_view>(*url_parts_.query)
                     : string_view{};
}

url::query_iterator url::query_begin() const noexcept {
  return has_query()? url::query_iterator{url_parts_.query} : url::query_iterator{};
}

url::query_iterator url::query_end() const noexcept {
  return url::query_iterator{};
}

bool url::has_fragment() const noexcept {
  return static_cast<bool>(url_parts_.fragment);
}

url::string_view url::fragment() const noexcept {
  return has_fragment() ? static_cast<string_view>(*url_parts_.fragment)
                        : string_view{};
}

std::string url::string() const { return url_; }

std::wstring url::wstring() const {
  return std::wstring(std::begin(*this), std::end(*this));
}

std::u16string url::u16string() const {
  return std::u16string(std::begin(*this), std::end(*this));
}

std::u32string url::u32string() const {
  return std::u32string(std::begin(*this), std::end(*this));
}

bool url::empty() const noexcept { return url_.empty(); }

bool url::is_absolute() const noexcept { return has_scheme(); }

bool url::is_opaque() const noexcept {
  return (is_absolute() && !has_host());
}

bool url::is_special() const noexcept {
  if (has_scheme()) {
    auto scheme = this->scheme();
    auto first = std::begin(special_schemes()), last = std::end(special_schemes());
    auto it = std::find_if(
        first, last,
        [&scheme](const std::pair<url::string_type, optional<std::uint16_t>>
                      &special_scheme) -> bool {
          return scheme == url::string_view(special_scheme.first);
        });
    return it != last;
  }
  return false;
}

optional<std::uint16_t> url::default_port(const string_type &scheme) {
  auto first = std::begin(special_schemes()), last = std::end(special_schemes());
  auto it = std::find_if(
      first, last,
      [&scheme](const std::pair<url::string_type, optional<std::uint16_t>>
                    &special_scheme) -> bool {
        return scheme == special_scheme.first;
      });
  if (it != last) {
    return it->second;
  }
  return nullopt;
}

namespace {
url::string_type serialize_host(url::string_view host) {
  return url::string_type{host};
}

url::string_type serialize_port(url::string_view port) {
  return url::string_type{port};
}
}  // namespace

url url::serialize() const {
  // https://url.spec.whatwg.org/#url-serializing
  auto result = string_type{};

  result += string_type(scheme());
  result += ":";

  if (has_host()) {
    result += "//";
    if (has_user_info()) {
      result += string_type(user_info());
      result += "@";
    }

    result += serialize_host(host());

    if (has_port()) {
      result += ":";
      result += serialize_port(port());
    }
  }
  // else if (scheme() == "file") {
  //   result += "//";
  // }

  auto path_it = path_begin();
  if (cannot_be_a_base_url_) {
    result += string_type(*path_it);
  }
  else {
    result += string_type(path());
  }

  if (has_query()) {
    result += "?";
    result += string_type(query());
  }

  if (has_fragment()) {
    result += "#";
    result += string_type(fragment());
  }

  return url{result};
}

int url::compare(const url &other) const {
  // if both URLs are empty, then we should define them as equal
  // even though they're still invalid.
  if (empty() && other.empty()) {
    return 0;
  }

  if (empty()) {
    return -1;
  }

  if (other.empty()) {
    return 1;
  }

  return serialize().url_.compare(other.serialize().url_);
}

bool url::initialize(const string_type &url) {
  url_ = ::network::detail::trim_copy(url);
  if (!url_.empty()) {
    url_view_ = string_view(url_);
    const_iterator it = std::begin(url_view_), last = std::end(url_view_);
    bool is_valid = ::network::detail::parse(it, last, url_parts_);
    return is_valid;
  }
  return true;
}

void swap(url &lhs, url &rhs) noexcept { lhs.swap(rhs); }

bool operator==(const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

bool operator==(const url &lhs, const char *rhs) noexcept {
  if (std::strlen(rhs) !=
      std::size_t(std::distance(std::begin(lhs), std::end(lhs)))) {
    return false;
  }
  return std::equal(std::begin(lhs), std::end(lhs), rhs);
}

bool operator<(const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) < 0;
}
}  // namespace whatwg
}  // namespace network