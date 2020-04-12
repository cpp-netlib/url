// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>
#include <cmath>
#include <locale>
#include <vector>
#include <algorithm>
#include <optional>
#include <cstdlib>
#include <cerrno>
#include <skyr/network/ipv4_address.hpp>

namespace skyr {
inline namespace v1 {
namespace {
class ipv4_address_error_category : public std::error_category {
 public:
  [[nodiscard]] auto name() const noexcept -> const char * override {
    return "ipv4 address";
  }

  [[nodiscard]] auto message(int error) const noexcept -> std::string override {
    switch (static_cast<ipv4_address_errc>(error)) {
      case ipv4_address_errc::too_many_segments:
        return "Input contains more than 4 segments";
      case ipv4_address_errc::empty_segment:
        return "Empty input";
      case ipv4_address_errc::invalid_segment_number:
        return "Invalid segment number";
      case ipv4_address_errc::overflow:
        return "Overflow";
      default:
        return "(Unknown error)";
    }
  }
};

const ipv4_address_error_category category{};
}  // namespace

auto make_error_code(ipv4_address_errc error) -> std::error_code {
  return std::error_code(static_cast<int>(error), category);
}

namespace {
auto parse_ipv4_number(
    std::string_view input,
    bool &validation_error_flag) -> tl::expected<std::uint64_t, std::error_code> {
  auto base = 10;

  if (
    (input.size() >= 2) && (input[0] == '0') &&
    (std::tolower(input[1], std::locale::classic()) == 'x')) {
    input = input.substr(2);
    base = 16;
  } else if ((input.size() >= 2) && (input[0] == '0')) {
    input = input.substr(1);
    base = 8;
  }

  if (input.empty()) {
    return 0ULL;
  }

  char *pos = const_cast<char*>(input.data()) + input.size();
  auto number = std::strtoull(input.data(), &pos, base);
  if (errno == ERANGE || (pos != input.data() + input.size())) {
    return tl::make_unexpected(
        make_error_code(ipv4_address_errc::invalid_segment_number));
  }
  return number;
}
}  // namespace

auto ipv4_address::to_string() const -> std::string {
  using namespace std::string_literals;

  auto output = ""s;

  auto n = address_;
  for (auto i = 1U; i <= 4U; ++i) {
    output = std::to_string(n % 256) + output;

    if (i != 4) {
      output = "." + output;
    }

    n = static_cast<std::uint32_t>(std::floor(n / 256.));
  }

  return output;
}

namespace details {
namespace {
auto parse_ipv4_address(std::string_view input) -> std::pair<tl::expected<ipv4_address, std::error_code>, bool> {
  auto validation_error_flag = false;
  auto validation_error = false;

  std::vector<std::string> parts;
  parts.emplace_back();
  for (auto ch : input) {
    if (ch == '.') {
      parts.emplace_back();
    } else {
      parts.back().push_back(ch);
    }
  }

  if (parts.back().empty()) {
    validation_error_flag = true;
    if (parts.size() > 1) {
      parts.pop_back();
    }
  }

  if (parts.size() > 4) {
    return
      std::make_pair(
          tl::make_unexpected(
              make_error_code(
                  ipv4_address_errc::too_many_segments)), true);
  }

  auto numbers = std::vector<std::uint64_t>();

  for (const auto &part : parts) {
    if (part.empty()) {
      return
        std::make_pair(
            tl::make_unexpected(
                make_error_code(
                    ipv4_address_errc::empty_segment)), true);
    }

    auto number = parse_ipv4_number(std::string_view(part), validation_error_flag);
    if (!number) {
      return
        std::make_pair(
            tl::make_unexpected(
                make_error_code(
                    ipv4_address_errc::invalid_segment_number)), validation_error_flag);
    }

    numbers.push_back(number.value());
  }

  if (validation_error_flag) {
    validation_error = true;
  }

  auto numbers_first = begin(numbers), numbers_last = end(numbers);

  auto numbers_it =
      std::find_if(numbers_first, numbers_last,
                   [](auto number) -> bool { return number > 255; });
  if (numbers_it != numbers_last) {
    validation_error = true;
  }

  auto numbers_last_but_one = numbers_last;
  --numbers_last_but_one;

  numbers_it = std::find_if(numbers_first, numbers_last_but_one,
                            [](auto number) -> bool { return number > 255; });
  if (numbers_it != numbers_last_but_one) {
    return
      std::make_pair(
          tl::make_unexpected(
              make_error_code(ipv4_address_errc::overflow)), true);
  }

  if (numbers.back() >=
      static_cast<std::uint64_t>(std::pow(256, 5 - numbers.size()))) {
    return
      std::make_pair(
          tl::make_unexpected(
              make_error_code(ipv4_address_errc::overflow)), true);
  }

  auto ipv4 = numbers.back();
  numbers.pop_back();

  auto counter = 0UL;
  for (auto number : numbers) {
    ipv4 += number * static_cast<std::uint64_t>(std::pow(256, 3 - counter));
    ++counter;
  }

  return std::make_pair(
      ipv4_address(static_cast<unsigned int>(ipv4)), validation_error);
}
}  // namespace
}  // namespace details

auto parse_ipv4_address(std::string_view input) -> tl::expected<ipv4_address, std::error_code> {
  return details::parse_ipv4_address(input).first;
}
}  // namespace v1
}  // namespace skyr
