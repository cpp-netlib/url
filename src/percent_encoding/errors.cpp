// Copyright 2019-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <skyr/percent_encoding/errors.hpp>

namespace skyr {
inline namespace v1 {
namespace percent_encoding {
namespace {
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

const percent_encode_error_category category{};
}  // namespace

auto make_error_code(percent_encode_errc error) noexcept -> std::error_code {
  return std::error_code(static_cast<int>(error), category);
}
}  // namespace percent_encoding
}  // namespace v1
}  // namespace skyr
