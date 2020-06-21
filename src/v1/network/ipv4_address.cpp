// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cmath>
#include <climits>
#include <locale>
#include <skyr/v1/network/ipv4_address.hpp>
#include <skyr/v1/string/split.hpp>
#include <skyr/v1/containers/static_vector.hpp>

namespace skyr { inline namespace v1 {
auto ipv4_address::serialize() const -> std::string  {
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

namespace {
inline auto parse_ipv4_number(
    std::string_view input,
    bool *validation_error) -> tl::expected<std::uint64_t, ipv4_address_errc> {
  auto base = 10;

  if ((input.size() >= 2) && (input[0] == '0') &&
      (std::tolower(input[1], std::locale::classic()) == 'x')) {
    *validation_error |= true;
    input = input.substr(2);
    base = 16;
  } else if ((input.size() >= 2) && (input[0] == '0')) {
    *validation_error |= true;
    input = input.substr(1);
    base = 8;
  }

  if (input.empty()) {
    return 0ULL;
  }

  char *pos = const_cast<char *>(input.data()) + input.size();  // NOLINT
  auto number = std::strtoull(input.data(), &pos, base);
  if ((number == ULLONG_MAX) || (pos != input.data() + input.size())) {
    return tl::make_unexpected(ipv4_address_errc::invalid_segment_number);
  }
  return number;
}
}  // namespace

auto parse_ipv4_address(
    std::string_view input, bool *validation_error) -> tl::expected<ipv4_address, ipv4_address_errc> {
  using namespace std::string_view_literals;

  static_vector<std::string_view, 8> parts;
  for (auto &&part : split(input, ".")) {
    if (parts.size() == parts.max_size()) {
      *validation_error |= true;
      return tl::make_unexpected(ipv4_address_errc::too_many_segments);
    }
    parts.emplace_back(part);
  }

  if (parts.back().empty()) {
    *validation_error |= true;
    if (parts.size() > 1) {
      parts.pop_back();
    }
  }

  if (parts.size() > 4) {
    *validation_error |= true;
    return tl::make_unexpected(ipv4_address_errc::too_many_segments);
  }

  auto numbers = static_vector<std::uint64_t, 4>{};

  for (const auto &part : parts) {
    if (part.empty()) {
      *validation_error |= true;
      return tl::make_unexpected(ipv4_address_errc::empty_segment);
    }

    auto number = parse_ipv4_number(std::string_view(part), validation_error);
    if (!number) {
      *validation_error |= true;
      return tl::make_unexpected(ipv4_address_errc::invalid_segment_number);
    }

    numbers.push_back(number.value());
  }

  constexpr static auto greater_than_255 = [] (auto number) { return number> 255; };

  auto numbers_first = std::begin(numbers), numbers_last = std::end(numbers);

  auto numbers_it = std::find_if(numbers_first, numbers_last, greater_than_255);
  if (numbers_it != numbers_last) {
    *validation_error |= true;
  }

  auto numbers_last_but_one = numbers_last;
  --numbers_last_but_one;

  numbers_it = std::find_if(numbers_first, numbers_last_but_one, greater_than_255);
  if (numbers_it != numbers_last_but_one) {
    return tl::make_unexpected(ipv4_address_errc::overflow);
  }

  if (numbers.back() >= static_cast<std::uint64_t>(std::pow(256, 5 - numbers.size()))) {
    *validation_error |= true;
    return tl::make_unexpected(ipv4_address_errc::overflow);
  }

  auto ipv4 = numbers.back();
  numbers.pop_back();

  auto counter = 0UL;
  for (auto &&number : numbers) {
    ipv4 += number * static_cast<std::uint64_t>(std::pow(256, 3 - counter));
    ++counter;
  }
  return ipv4_address(static_cast<unsigned int>(ipv4));
}
}  // namespace v1
}  // namespace skyr
