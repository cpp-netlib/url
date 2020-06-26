// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_DOMAIN_IDNA_ITERATOR_HPP
#define SKYR_V1_DOMAIN_IDNA_ITERATOR_HPP

#include <iterator>
#include <skyr/v1/domain/errors.hpp>
#include <skyr/v1/domain/idna.hpp>
#include <skyr/v1/unicode/code_point.hpp>
#include <skyr/v1/unicode/ranges/sentinel.hpp>


namespace skyr {
inline namespace v1 {
namespace idna {
/// An iterator adapter that converts a domain name.
/// \tparam Iterator
/// \tparam Sentinel
template <
    class Iterator,
    class Sentinel=unicode::sentinel
    >
class idna_code_point_map_iterator {
 public:

  using iterator_category = std::forward_iterator_tag;
  using value_type = tl::expected<char32_t, domain_errc>;
  using const_reference = value_type;
  using reference = const_reference;
  using const_pointer = const value_type *;
  using pointer = const_pointer;
  using difference_type = typename Iterator::difference_type;
  using size_type = typename Iterator::size_type;

  /// Constructor
  /// \param first
  /// \param last
  /// \param use_std3_ascii_rules
  /// \param transitional_processing
  explicit constexpr idna_code_point_map_iterator(
      Iterator first,
      Sentinel last,
      bool use_std3_ascii_rules,
      bool transitional_processing)
  : it_(first)
  , last_(last)
  , use_std3_ascii_rules_(use_std3_ascii_rules)
  , transitional_processing_(transitional_processing) {}

  ///
  /// \return
  constexpr auto operator ++ () noexcept -> idna_code_point_map_iterator & {
    increment();
    return *this;
  }

  ///
  /// \return
  constexpr auto operator ++ (int) noexcept -> idna_code_point_map_iterator {
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  constexpr auto operator * () const noexcept -> const_reference {
    constexpr auto map_code_point = [] (char32_t code_point, bool use_std3_ascii_rules, bool transitional_processing)
        -> tl::expected<char32_t, domain_errc> {
      switch (idna::code_point_status(code_point)) {
        case idna::idna_status::disallowed:
          return tl::make_unexpected(domain_errc::disallowed_code_point);
        case idna::idna_status::disallowed_std3_valid:
          if (use_std3_ascii_rules) {
            return tl::make_unexpected(domain_errc::disallowed_code_point);
          } else {
            return code_point;
          }
          break;
        case idna::idna_status::disallowed_std3_mapped:
          if (use_std3_ascii_rules) {
            return tl::make_unexpected(domain_errc::disallowed_code_point);
          } else {
            return idna::map_code_point(code_point);
          }
        case idna::idna_status::ignored:
          return tl::make_unexpected(domain_errc::disallowed_code_point);
        case idna::idna_status::mapped:
          return idna::map_code_point(code_point);
        case idna::idna_status::deviation:
          if (transitional_processing) {
            return idna::map_code_point(code_point);
          } else {
            return code_point;
          }
        case idna::idna_status::valid:
          return code_point;
      }
      return code_point;
    };

    auto code_point = unicode::u32_value(*it_);
    if (code_point) {
      return map_code_point(code_point.value(), use_std3_ascii_rules_, transitional_processing_);
    }
    else {
      return tl::make_unexpected(domain_errc::encoding_error);
    }
  }

  ///
  /// \param sentinel
  /// \return
  [[nodiscard]] constexpr auto operator == ([[maybe_unused]] unicode::sentinel sentinel) const noexcept {
    return it_ == last_;
  }

  ///
  /// \param sentinel
  /// \return
  [[nodiscard]] constexpr auto operator != (unicode::sentinel sentinel) const noexcept {
    return !(*this == sentinel);
  }

 private:

  constexpr void increment() {
    constexpr auto is_ignored = [] (auto code_point) {
      auto value = unicode::u32_value(code_point);
      return (value && idna::code_point_status(value.value()) == idna::idna_status::ignored);
    };

    ++it_;
    while ((it_ != last_) && is_ignored(*it_)) {
      ++it_;
    }
  }

  Iterator it_;
  Sentinel last_;
  bool use_std3_ascii_rules_;
  bool transitional_processing_;

};

/// A range adapter that maps code points in a domain name using IDNA
/// \tparam DomainName
template <class DomainName>
class idna_code_point_map_range {
 public:

  ///
  /// \param domain_name
  /// \param use_std3_ascii_rules
  /// \param transitional_processing
  constexpr idna_code_point_map_range(
      DomainName &&domain_name,
      bool use_std3_ascii_rules,
      bool transitional_processing)
  : domain_name_(domain_name)
  , use_std3_ascii_rules_(use_std3_ascii_rules)
  , transitional_processing_(transitional_processing) {}

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return idna_code_point_map_iterator<unicode::traits::range_iterator_t<DomainName>>(
        std::cbegin(domain_name_), std::cend(domain_name_), use_std3_ascii_rules_, transitional_processing_);
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cend() const noexcept {
    return unicode::sentinel{};
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto begin() const noexcept {
    return cbegin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto end() const noexcept {
    return cend();
  }

 private:
  DomainName domain_name_;
  bool use_std3_ascii_rules_;
  bool transitional_processing_;

};

namespace views {
///
/// \tparam DomainName
/// \param domain_name
/// \param use_std3_ascii_rules
/// \param transitional_processing
/// \return A range adapter
template <class DomainName>
constexpr inline auto map_code_points(
    DomainName &&domain_name, bool use_std3_ascii_rules, bool transitional_processing)
    -> idna_code_point_map_range<DomainName> {
  return {domain_name, use_std3_ascii_rules, transitional_processing};
}
}  // namespaec views
}  // namespace idna
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_DOMAIN_IDNA_ITERATOR_HPP
