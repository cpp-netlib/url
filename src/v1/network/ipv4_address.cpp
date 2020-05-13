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
#include <cerrno>
#include <skyr/v1/network/ipv4_address.hpp>
#include <skyr/v1/ranges/string_element_range.hpp>

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

auto parse_ipv4_address(std::string_view input, bool *validation_error) -> tl::expected<ipv4_address, std::error_code> {
  using namespace std::string_view_literals;

  auto validation_error_flag = false;

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
    *validation_error |= true;
    return tl::make_unexpected(make_error_code(ipv4_address_errc::too_many_segments));
  }

  auto numbers = std::vector<std::uint64_t>();

  for (const auto &part : parts) {
    if (part.empty()) {
      *validation_error |= true;
      return tl::make_unexpected(make_error_code(ipv4_address_errc::empty_segment));
    }

    auto number = parse_ipv4_number(std::string_view(part), validation_error_flag);
    if (!number) {
      *validation_error |= validation_error_flag;
      return tl::make_unexpected(make_error_code(ipv4_address_errc::invalid_segment_number));
    }

    numbers.push_back(number.value());
  }

  if (validation_error_flag) {
    *validation_error |= true;
  }

  auto numbers_first = begin(numbers), numbers_last = end(numbers);

  auto numbers_it =
      std::find_if(numbers_first, numbers_last,
                   [](auto number) -> bool { return number > 255; });
  if (numbers_it != numbers_last) {
    *validation_error |= true;
  }

  auto numbers_last_but_one = numbers_last;
  --numbers_last_but_one;

  numbers_it = std::find_if(numbers_first, numbers_last_but_one,
                            [](auto number) -> bool { return number > 255; });
  if (numbers_it != numbers_last_but_one) {
    return tl::make_unexpected(make_error_code(ipv4_address_errc::overflow));
  }

  if (numbers.back() >=
      static_cast<std::uint64_t>(std::pow(256, 5 - numbers.size()))) {
    *validation_error |= true;
    return tl::make_unexpected(make_error_code(ipv4_address_errc::overflow));
  }

  auto ipv4 = numbers.back();
  numbers.pop_back();

  auto counter = 0UL;
  for (auto number : numbers) {
    ipv4 += number * static_cast<std::uint64_t>(std::pow(256, 3 - counter));
    ++counter;
  }
  return ipv4_address(static_cast<unsigned int>(ipv4));
}
}  // namespace v1
}  // namespace skyr
