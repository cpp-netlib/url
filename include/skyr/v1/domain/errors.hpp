// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_DOMAIN_ERRORS_HPP
#define SKYR_V1_DOMAIN_ERRORS_HPP

#include <system_error>

namespace skyr {
inline namespace v1 {
/// \enum domain_errc
/// Enumerates domain processing errors
enum class domain_errc {
  /// The domain code point is disallowed
  disallowed_code_point = 1,
  /// The encoder or decoder received bad input
  bad_input,
  /// Overflow
  overflow,
  /// Unicode encoding error
  encoding_error,
  /// Invalid domain name length
  invalid_length,
};

namespace details {
class domain_error_category : public std::error_category {
 public:
  [[nodiscard]] auto name() const noexcept -> const char * override {
    return "domain";
  }

  [[nodiscard]] auto message(int error) const noexcept -> std::string override {
    switch (static_cast<domain_errc>(error)) {
      case domain_errc::disallowed_code_point:return "Disallowed code point";
      case domain_errc::bad_input:return "Bad input";
      case domain_errc::overflow:return "Overflow";
      case domain_errc::encoding_error:return "Encoding error";
      case domain_errc::invalid_length:return "Invalid domain name length";
      default:return "(Unknown error)";
    }
  }
};
}  // namespace details

/// Creates a `std::error_code` given a `skyr::domain_errc` value
/// \param error A domain error
/// \returns A `std::error_code` object
inline auto make_error_code(domain_errc error) noexcept -> std::error_code {
  static const details::domain_error_category category{};
  return std::error_code(static_cast<int>(error), category);
}
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::domain_errc> : true_type {};
}  // namespace std

#endif //SKYR_V1_DOMAIN_ERRORS_HPP
