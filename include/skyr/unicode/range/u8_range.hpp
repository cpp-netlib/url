// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_U8_RANGE_HPP
#define SKYR_U8_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <tl/expected.hpp>
#include <range/v3/view.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>

namespace skyr::unicode {
/// This class defines a range over a code point in raw bytes,
/// according to UTF-8.
/// \tparam OctetIterator An iterator type over the raw bytes
template <typename OctetIterator>
class u8_code_point_t {
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

  /// \brief Constructor
  /// \param first An iterator at the beginning of the code point
  /// \param last An iterator at the end of the code point
  constexpr u8_code_point_t(
      OctetIterator first,
      OctetIterator last)
      : first(first)
      , last(last) {}

  /// \brief Constructor. The length of the code point sequence is
  ///        inferred from the first code point value.
  /// \param first An iterator at the beginning of the code point
  explicit constexpr u8_code_point_t(OctetIterator first)
      : u8_code_point_t(first, first + sequence_length(*first)) {}

  /// \brief Copy constructor.
  constexpr u8_code_point_t(const u8_code_point_t &) = default;
  /// \brief Move constructor.
  constexpr u8_code_point_t(u8_code_point_t &&) noexcept = default;
  /// \brief Copy assignment operator.
  constexpr u8_code_point_t &operator=(const u8_code_point_t &) = default;
  /// \brief Move assignment operator.
  constexpr u8_code_point_t &operator=(u8_code_point_t &&) noexcept = default;
  /// \brief Destructor.
  ~u8_code_point_t() = default;

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

  /// \brief Returns the length in bytes of this code point.
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
inline tl::expected<u8_code_point_t<typename OctetRange::const_iterator>, std::error_code> u8_code_point(
    const OctetRange &range) {
  auto first = std::begin(range), last = std::end(range);
  if (std::distance(first, last) > sequence_length(*first)) {
    return tl::make_unexpected(make_error_code(unicode_errc::overflow));
  }
  return u8_code_point_t<typename OctetRange::const_iterator>(
      first,
      first + sequence_length(*first));
}

/// Tests if the code point value is valid.
/// \returns \c true if the value is a valid code point, \c false otherwise
template <typename OctetIterator>
inline bool is_valid(const u8_code_point_t<OctetIterator> &code_point) {
  return static_cast<bool>(find_code_point(std::begin(code_point)));
}

///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
inline tl::expected<u8_code_point_t<typename OctetRange::const_iterator>, std::error_code> valid_u8_code_point(
    const OctetRange &range) {
  using result_type = tl::expected<u8_code_point_t<typename OctetRange::const_iterator>, std::error_code>;

  auto check_code_point = [] (auto &&code_point) -> result_type {
    return find_code_point(std::begin(code_point))
        .and_then([=] (auto) -> result_type {
          return code_point;
        });
  };

  return
      u8_code_point(range)
      .and_then(check_code_point);
}

///
/// \tparam OctetIterator
template <typename OctetIterator>
class unchecked_u8_range_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = u8_code_point_t<OctetIterator>;
  ///
  using reference = value_type;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  constexpr unchecked_u8_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr unchecked_u8_range_iterator(OctetIterator it)
      : it_(it) {}
  ///
  constexpr unchecked_u8_range_iterator(const unchecked_u8_range_iterator&) = default;
  ///
  constexpr unchecked_u8_range_iterator(unchecked_u8_range_iterator&&) noexcept = default;
  ///
  constexpr unchecked_u8_range_iterator &operator=(const unchecked_u8_range_iterator&) = default;
  ///
  constexpr unchecked_u8_range_iterator &operator=(unchecked_u8_range_iterator&&) noexcept = default;
  ///
  ~unchecked_u8_range_iterator() = default;

  ///
  /// \return
  unchecked_u8_range_iterator operator ++ (int) {
    assert(it_);
    auto result = *this;
    std::advance(it_.value(), sequence_length(*it_.value()));
    return result;
  }

  ///
  /// \return
  unchecked_u8_range_iterator &operator ++ () {
    assert(it_);
    std::advance(it_.value(), sequence_length(*it_.value()));
    return *this;
  }

  ///
  /// \return
  constexpr reference operator * () const noexcept {
    assert(it_);
    return u8_code_point_t<OctetIterator>(
        it_.value(),
        it_.value() + sequence_length(*it_.value()));
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator == (const unchecked_u8_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator != (const unchecked_u8_range_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  std::optional<OctetIterator> it_;

};

///
/// \tparam OctetIterator
template <typename OctetIterator>
class u8_range_iterator {

  using iterator_type = unchecked_u8_range_iterator<OctetIterator>;

 public:

  ///
  using iterator_category = typename iterator_type::iterator_category;
  ///
  using value_type = tl::expected<typename iterator_type::value_type, std::error_code>;
  ///
  using reference = value_type;
  ///
  using pointer = typename std::add_pointer<value_type>::type;
  ///
  using difference_type = typename iterator_type::difference_type;

  ///
  constexpr u8_range_iterator() = default;
  ///
  /// \param it
  explicit constexpr u8_range_iterator(OctetIterator it, OctetIterator last)
      : it_(it)
      , last_(last) {}
  ///
  constexpr u8_range_iterator(const u8_range_iterator&) = default;
  ///
  constexpr u8_range_iterator(u8_range_iterator&&) noexcept = default;
  ///
  constexpr u8_range_iterator &operator=(const u8_range_iterator&) = default;
  ///
  constexpr u8_range_iterator &operator=(u8_range_iterator&&) noexcept = default;
  ///
  ~u8_range_iterator() = default;

  ///
  /// \return
  u8_range_iterator operator ++ (int) {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  u8_range_iterator &operator ++ () {
    increment();
    return *this;
  }

  ///
  /// \return
  constexpr reference operator * () const noexcept {
    return valid_u8_code_point(*it_);
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator == (const u8_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator != (const u8_range_iterator &other) const noexcept {
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
/// \tparam OctetRange
template <class OctetRange>
class view_unchecked_u8_range
    : public ranges::view_base {

  using octet_iterator_type = typename OctetRange::const_iterator;
  using iterator_type = unchecked_u8_range_iterator<octet_iterator_type>;

 public:

  ///
  using value_type = u8_code_point_t<octet_iterator_type>;
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

  /// Default constructor
  constexpr view_unchecked_u8_range() = default;

  ///
  /// \param range
  explicit constexpr view_unchecked_u8_range(const OctetRange &range)
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
/// \tparam OctetRange
template <class OctetRange>
class view_u8_range
    : public ranges::view_base {

  using octet_iterator_type = typename OctetRange::const_iterator;
  using iterator_type = u8_range_iterator<octet_iterator_type>;

 public:

  ///
  using value_type = u8_code_point_t<octet_iterator_type>;
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
  constexpr view_u8_range() = default;

  ///
  /// \param range
  explicit constexpr view_u8_range(const OctetRange &range)
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
struct unchecked_u8_range_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(OctetRange &&range) const {
    return view_unchecked_u8_range{std::forward<OctetRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(OctetRange &&range, const unchecked_u8_range_fn&) {
    return view_unchecked_u8_range{std::forward<OctetRange>(range)};
  }
};

///
struct u8_range_fn {
  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  constexpr auto operator()(OctetRange &&range) const {
    return view_u8_range{std::forward<OctetRange>(range)};
  }

  ///
  /// \tparam OctetRange
  /// \param range
  /// \return
  template <typename OctetRange>
  friend constexpr auto operator|(OctetRange &&range, const u8_range_fn&) {
    return view_u8_range{std::forward<OctetRange>(range)};
  }
};

namespace view {
///
static constexpr unchecked_u8_range_fn unchecked_u8;
///
static constexpr u8_range_fn u8;
}  // namespace view
}  // namespace skyr::unicode

#endif //SKYR_U8_RANGE_HPP
