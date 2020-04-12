// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/domain/errors.hpp>

namespace skyr {
inline namespace v1 {
namespace {
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
      default:return "(Unknown error)";
    }
  }
};

const domain_error_category category{};
}  // namespace

auto make_error_code(domain_errc error) noexcept -> std::error_code {
  return std::error_code(static_cast<int>(error), category);
}
}  // namespace v1
}  // namespace skyr
