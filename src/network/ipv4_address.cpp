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
#include <skyr/ranges/string_element_range.hpp>

namespace skyr {
inline namespace v1 {
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

  char *pos = const_cast<char *>(input.data()) + input.size();  // NOLINT
  auto number = std::strtoull(input.data(), &pos, base);
  if (errno == ERANGE || (pos != input.data() + input.size())) {
    return tl::make_unexpected(
        make_error_code(ipv4_address_errc::invalid_segment_number));
  }
  return number;
}
}  // namespace

auto ipv4_address::serialize() const -> std::string {
  using namespace std::string_literals;

  auto output = ""s;

  auto n = address_;
  for (auto i = 1U; i <= 4U; ++i) {
    output = std::to_string(n % 256) + output; // NOLINT

    if (i != 4) {
      output = "." + output; // NOLINT
    }

    n = static_cast<std::uint32_t>(std::floor(n / 256.));
  }

  return output;
}

namespace details {
namespace {
auto parse_ipv4_address(std::string_view input)
    -> std::pair<tl::expected<ipv4_address, std::error_code>, bool> {

  auto validation_error_flag = false;
  auto validation_error = false;

  std::vector<std::string_view> parts;
  constexpr static auto separator = [] (char c) { return c == '.'; };
  for (auto part : split(input, separator)) {
    parts.emplace_back(part);
  }

  if (parts.empty()) {
    return std::make_pair(
        tl::make_unexpected(
            make_error_code(
                ipv4_address_errc::empty_segment)), true);
  }

  if (parts.size() > 4) {
    return
      std::make_pair(
          tl::make_unexpected(
              make_error_code(
                  ipv4_address_errc::too_many_segments)), true);
  }

  auto numbers = std::vector<std::uint64_t>();

  for (auto part : parts) {
    if (part.empty()) {
      return
        std::make_pair(
            tl::make_unexpected(
                make_error_code(
                    ipv4_address_errc::empty_segment)), true);
    }

    auto number = parse_ipv4_number(part, validation_error_flag);
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

  constexpr static auto valid_segment = [] (auto number) { return number > 255; };

  auto numbers_first = begin(numbers), numbers_last = end(numbers);

  auto numbers_it = std::find_if(numbers_first, numbers_last, valid_segment);
  if (numbers_it != numbers_last) {
    validation_error = true;
  }

  auto numbers_last_but_one = numbers_last;
  --numbers_last_but_one;

  numbers_it = std::find_if(numbers_first, numbers_last_but_one, valid_segment);
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
