// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_PERCENT_ENCODING_ERRORS_HPP
#define SKYR_PERCENT_ENCODING_ERRORS_HPP

#include <system_error>
#include <type_traits>

namespace skyr {
inline namespace v1 {
namespace percent_encoding {
/// Enumerates percent encoding errors
enum class percent_encode_errc {
  /// Input was not a hex value
  non_hex_input,
  /// Overflow
  overflow,
};

namespace details {
class percent_encode_error_category : public std::error_category {
 public:
  [[nodiscard]] auto name() const noexcept -> const char * override {
    return "percent encoding";
  }

  [[nodiscard]] auto message(int error) const noexcept -> std::string override {
    switch (static_cast<percent_encode_errc>(error)) {
      case percent_encode_errc::non_hex_input:return "Non hex input";
      case percent_encode_errc::overflow:return "Overflow";
      default:return "(Unknown error)";
    }
  }
};
}  // namespace

/// Creates a `std::error_code` given a `skyr::percent_encode_errc`
/// value
/// \param error A percent encoding error
/// \returns A `std::error_code` object
inline auto make_error_code(percent_encode_errc error) noexcept -> std::error_code {
  static const details::percent_encode_error_category category{};
  return std::error_code(static_cast<int>(error), category);
}
}  // namespace percent_encoding
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::percent_encoding::percent_encode_errc>
    : true_type {};
}  // namespace std

#endif  // SKYR_PERCENT_ENCODING_ERRORS_HPP
