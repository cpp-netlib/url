// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>
#include <cmath>
#include <locale>
#include <array>
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

namespace details {
namespace {
auto parse_ipv4_address(std::string_view input)
    -> std::pair<tl::expected<ipv4_address, std::error_code>, bool> {
  using namespace std::string_view_literals;

  auto validation_error_flag = false;
  auto validation_error = false;

  std::array<std::string_view, 4> parts;
  auto count = 0UL;
  for (auto part : split(input, "."sv)) {
    if (count >= parts.size()) {
      return
          std::make_pair(
              tl::make_unexpected(
                  make_error_code(
                      ipv4_address_errc::too_many_segments)), true);
    }

    parts[count] = part; // NOLINT
    ++count;
  }

  if (count == 0) {
    return std::make_pair(
        tl::make_unexpected(
            make_error_code(
                ipv4_address_errc::empty_segment)), true);
  }

  auto numbers = std::array<std::uint64_t, 4>({0, 0, 0, 0});

  for (auto i = 0UL; i < count; ++i) {
    auto part = parts[i]; // NOLINT
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

    numbers[i] = number.value(); // NOLINT
  }

  if (validation_error_flag) {
    validation_error = true;
  }

  constexpr static auto invalid_segment = [] (auto number) { return number > 255; };

  auto numbers_first = begin(numbers), numbers_last = begin(numbers);
  std::advance(numbers_last, count);

  auto numbers_it = std::find_if(numbers_first, numbers_last, invalid_segment);
  if (numbers_it != numbers_last) {
    validation_error = true;
  }

  auto numbers_last_but_one = numbers_last;
  std::advance(numbers_last_but_one, -1);

  numbers_it = std::find_if(numbers_first, numbers_last_but_one, invalid_segment);
  if (numbers_it != numbers_last_but_one) {
    return
      std::make_pair(
          tl::make_unexpected(
              make_error_code(ipv4_address_errc::overflow)), true);
  }

  if (numbers[count - 1] >= // NOLINT
      static_cast<std::uint64_t>(std::pow(256, 5 - count))) {
    return
      std::make_pair(
          tl::make_unexpected(
              make_error_code(ipv4_address_errc::overflow)), true);
  }

  auto ipv4 = numbers[count - 1]; // NOLINT
  --count;

  for (auto i = 0UL; i < count; ++i) {
    ipv4 += numbers[i] * static_cast<std::uint64_t>(std::pow(256, 3 - i)); // NOLINT
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
