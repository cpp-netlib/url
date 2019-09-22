// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_OCTET_RANGE_HPP
#define SKYR_OCTET_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <tl/expected.hpp>
#include <range/v3/view.hpp>
#include <skyr/unicode/unicode.hpp>

namespace skyr::unicode {
///
/// \tparam OctetIterator
template <typename OctetIterator>
class code_point_octet_t {
 public:

  ///
  using const_iterator = OctetIterator;
  ///
  using iterator = const_iterator;
  ///
  using value_type = char;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using size_type = std::size_t;

  ///
  /// \param first
  constexpr code_point_octet_t(
      OctetIterator first,
      OctetIterator last)
      : first(first)
      , last(last) {}

  ///
  /// \param first
  explicit constexpr code_point_octet_t(OctetIterator first)
      : code_point_octet_t(first, first + sequence_length(*first)) {}

  ///
  constexpr code_point_octet_t(const code_point_octet_t &) = default;
  ///
  constexpr code_point_octet_t(code_point_octet_t &&) noexcept = default;
  ///
  constexpr code_point_octet_t &operator=(const code_point_octet_t &) = default;
  ///
  constexpr code_point_octet_t &operator=(code_point_octet_t &&) noexcept = default;
  ///
  ~code_point_octet_t() = default;

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return first;
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return last;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return begin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cend() const noexcept {
    return end();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return sequence_length(*first);
  }

 private:

  OctetIterator first, last;

};

///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
inline tl::expected<code_point_octet_t<typename OctetRange::const_iterator>, unicode_errc> code_point_octets(
    const OctetRange &range) {
  auto first = std::begin(range), last = std::end(range);
  if (std::distance(first, last) > sequence_length(*first)) {
    return tl::make_unexpected(unicode_errc::overflow);
  }
  return code_point_octet_t<typename OctetRange::const_iterator>(
      first,
      first + sequence_length(*first));
}

template <typename OctetIterator>
inline bool is_valid(const code_point_octet_t<OctetIterator> &code_point) {
  return static_cast<bool>(find_code_point(std::begin(code_point)));
}

///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
inline tl::expected<code_point_octet_t<typename OctetRange::const_iterator>, unicode_errc> valid_code_point(
    const OctetRange &range) {
  using result_type = tl::expected<code_point_octet_t<typename OctetRange::const_iterator>, unicode_errc>;

  auto check_code_point = [] (auto &&code_point) -> result_type {
    return find_code_point(std::begin(code_point))
        .and_then([=] (auto) -> result_type {
          return code_point;
        })
        .or_else([] (auto &&error) -> result_type {
          return tl::make_unexpected(error);
        });
  };

  return
      code_point_octets(range)
  .and_then(check_code_point);
}

///
template <typename OctetIterator>
class unchecked_octet_range_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = code_point_octet_t<OctetIterator>;
  ///
  using reference = value_type;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  constexpr unchecked_octet_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr unchecked_octet_range_iterator(OctetIterator it)
      : it_(it) {}
  ///
  constexpr unchecked_octet_range_iterator(const unchecked_octet_range_iterator&) = default;
  ///
  constexpr unchecked_octet_range_iterator(unchecked_octet_range_iterator&&) noexcept = default;
  ///
  constexpr unchecked_octet_range_iterator &operator=(const unchecked_octet_range_iterator&) = default;
  ///
  constexpr unchecked_octet_range_iterator &operator=(unchecked_octet_range_iterator&&) noexcept = default;
  ///
  ~unchecked_octet_range_iterator() = default;

  ///
  /// \return
  unchecked_octet_range_iterator operator ++ (int) {
    assert(it_);
    auto result = *this;
    std::advance(it_.value(), sequence_length(*it_.value()));
    return result;
  }

  ///
  /// \return
  unchecked_octet_range_iterator &operator ++ () {
    assert(it_);
    std::advance(it_.value(), sequence_length(*it_.value()));
    return *this;
  }

  ///
  /// \return
  constexpr reference operator * () const noexcept {
    assert(it_);
    return code_point_octet_t<OctetIterator>(
        it_.value(),
        it_.value() + sequence_length(*it_.value()));
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator == (const unchecked_octet_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator != (const unchecked_octet_range_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  std::optional<OctetIterator> it_;

};

///
template <typename OctetIterator>
class octet_range_iterator {

  using iterator_type = unchecked_octet_range_iterator<OctetIterator>;

 public:

  ///
  using iterator_category = typename iterator_type::iterator_category;
  ///
  using value_type = tl::expected<typename iterator_type::value_type, unicode_errc>;
  ///
  using reference = value_type;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  ///
  using difference_type = typename iterator_type::difference_type;

  ///
  constexpr octet_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr octet_range_iterator(OctetIterator it, OctetIterator last)
      : it_(it)
      , last_(last) {}
  ///
  constexpr octet_range_iterator(const octet_range_iterator&) = default;
  ///
  constexpr octet_range_iterator(octet_range_iterator&&) noexcept = default;
  ///
  constexpr octet_range_iterator &operator=(const octet_range_iterator&) = default;
  ///
  constexpr octet_range_iterator &operator=(octet_range_iterator&&) noexcept = default;
  ///
  ~octet_range_iterator() = default;

  ///
  /// \return
  octet_range_iterator operator ++ (int) {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  octet_range_iterator &operator ++ () {
    increment();
    return *this;
  }

  ///
  /// \return
  constexpr reference operator * () const noexcept {
    return valid_code_point(*it_);
//    using result_type = tl::expected<code_point_octet_t<OctetIterator>, unicode_errc>;
//
//    return
//    valid_code_point(*it_)
//    .or_else([=] (auto) -> result_type {
//      auto first = std::begin(*it_);
//      return code_point_octet(ranges::iterator_range(first, first));
//    })
//    .value();
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator == (const octet_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator != (const octet_range_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    if (**this) {
      ++it_;
      if (it_ == last_) {
        it_ = iterator_type();
      }
    } else {
      it_ = iterator_type();
    }
  }

  iterator_type it_, last_;

};

///
template <class OctetRange>
class view_unchecked_octet_range
    : public ranges::view_base {

  using octet_iterator_type = typename OctetRange::const_iterator;
  using iterator_type = unchecked_octet_range_iterator<octet_iterator_type>;

 public:

  ///
  using value_type = code_point_octet_t<octet_iterator_type>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_iterator = iterator_type;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  constexpr view_unchecked_octet_range() = default;

  ///
  /// \param range
  explicit constexpr view_unchecked_octet_range(const OctetRange &range)
      : impl_(
      impl(std::begin(range),
           std::end(range))) {}

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return impl_? impl_.value().first : iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return impl_? impl_.value().last : iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return begin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cend() const noexcept {
    return end();
  }

  ///
  /// \return
  [[nodiscard]] constexpr bool empty() const noexcept {
    return begin() == end();
  }

  ///
  /// \return
  [[nodiscard]] size_type size() const noexcept {
    return static_cast<size_type>(std::distance(begin(), end()));
  }

 private:

  struct impl {
    constexpr impl(
        octet_iterator_type first,
        octet_iterator_type last)
        : first(first)
        , last(last) {}
    iterator_type first, last;
  };

  std::optional<impl> impl_;

};

///
template <class OctetRange>
class view_octet_range
    : public ranges::view_base {

  using octet_iterator_type = typename OctetRange::const_iterator;
  using iterator_type = octet_range_iterator<octet_iterator_type>;

 public:

  ///
  using value_type = code_point_octet_t<octet_iterator_type>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_iterator = iterator_type;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  ///
  constexpr view_octet_range() = default;

  ///
  /// \param range
  explicit constexpr view_octet_range(const OctetRange &range)
      : impl_(
      impl(std::begin(range),
           std::end(range))) {}

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return impl_? impl_.value().first : iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return begin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cend() const noexcept {
    return end();
  }

  ///
  /// \return
  [[nodiscard]] constexpr bool empty() const noexcept {
    return begin() == end();
  }

  ///
  /// \return
  [[nodiscard]] size_type size() const noexcept {
    return static_cast<size_type>(std::distance(begin(), end()));
  }

 private:

  struct impl {
    constexpr impl(
        octet_iterator_type first,
        octet_iterator_type last)
        : first(first, last)
        , last(last, last) {}
    iterator_type first, last;
  };

  std::optional<impl> impl_;

};

///
struct unchecked_octet_range_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(OctetRange &&range) const {
    return view_unchecked_octet_range{std::forward<OctetRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(OctetRange &&range, const unchecked_octet_range_fn&) {
    return view_unchecked_octet_range{std::forward<OctetRange>(range)};
  }
};

///
struct octet_range_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(OctetRange &&range) const {
    return view_octet_range{std::forward<OctetRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(OctetRange &&range, const octet_range_fn&) {
    return view_octet_range{std::forward<OctetRange>(range)};
  }
};

namespace view {
///
//static constexpr unchecked_octet_range_fn u8;
static constexpr octet_range_fn u8;
}  // namespace view
}  // namespace skyr::unicode

#endif //SKYR_OCTET_RANGE_HPP
