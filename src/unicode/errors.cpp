// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/unicode/errors.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
namespace {
class unicode_error_category : public std::error_category {
 public:
  [[nodiscard]] const char *name() const noexcept override {
    return "unicode";
  }

  [[nodiscard]] std::string message(int error) const noexcept override {
    switch (static_cast<unicode_errc>(error)) {
      case unicode_errc::overflow:return "Overflow";
      case unicode_errc::invalid_lead:return "Invalid lead";
      case unicode_errc::illegal_byte_sequence:return "Illegal byte sequence";
      case unicode_errc::invalid_code_point:return "Invalid code point";
      default:return "(Unknown error)";
    }
  }
};

const unicode_error_category category{};
}  // namespace

std::error_code make_error_code(unicode_errc error) noexcept {
  return std::error_code(static_cast<int>(error), category);
}
}  // namespace unicode
}  // namespace v1
}  // namespace skyr
