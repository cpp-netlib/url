// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_NETWORK_IPV4_ADDRESS_HPP
#define SKYR_V2_NETWORK_IPV4_ADDRESS_HPP

#include <array>
#include <string>
#include <string_view>
#include <optional>
#include <cmath>
#include <locale>
#include <tl/expected.hpp>
#include <skyr/v2/platform/endianness.hpp>
#include <skyr/v2/containers/static_vector.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/drop_last.hpp>
#include <fmt/format.h>

namespace skyr::inline v2 {
/// Enumerates IPv4 address parsing errors
enum class ipv4_address_errc {
  /// The input contains more than 4 segments
  too_many_segments,
  /// The input contains an empty segment
  empty_segment,
  /// The segment numers invalid
  invalid_segment_number,
  /// Overflow
  overflow,
};

/// Represents an IPv4 address
class ipv4_address {
  unsigned int address_ = 0;

 public:
  /// Constructor
  constexpr ipv4_address() = default;

  /// Constructor
  /// \param address Sets the IPv4 address to `address`
  constexpr explicit ipv4_address(unsigned int address) : address_(to_network_byte_order(address)) {
  }

  /// The address value
  /// \returns The address value
  [[nodiscard]] constexpr auto address() const noexcept {
    return from_network_byte_order(address_);
  }

  /// The address in bytes in network byte order
  /// \returns The address in bytes
  [[nodiscard]] constexpr auto to_bytes() const noexcept -> std::array<unsigned char, 4> {
    return {{static_cast<unsigned char>(address_ >> 24u), static_cast<unsigned char>(address_ >> 16u),
             static_cast<unsigned char>(address_ >> 8u), static_cast<unsigned char>(address_)}};
  }

  /// \returns The address as a string
  [[nodiscard]] auto serialize() const -> std::string {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    constexpr auto separator = [](auto i) { return (i < 4) ? "."sv : ""sv; };

    auto output = ""s;
    auto n = address_;
    for (auto i = 1U; i <= 4U; ++i) {
      output = fmt::format("{}{}{}", separator(i), n % 256, output);
      n >>= 8;
    }
    return output;
  }
};

namespace details {
constexpr inline auto parse_ipv4_number(std::string_view input, bool *validation_error)
    -> tl::expected<std::uint64_t, ipv4_address_errc> {
  auto base = 10;

  if ((input.size() >= 2) && (input[0] == '0') && (std::tolower(input[1], std::locale::classic()) == 'x')) {
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
}  // namespace details

/// Parses an IPv4 address
/// \param input An input string
/// \returns An `ipv4_address` object or an error
constexpr inline auto parse_ipv4_address(std::string_view input, bool *validation_error)
    -> tl::expected<ipv4_address, ipv4_address_errc> {
  using namespace std::string_view_literals;

  constexpr auto to_string_view = [](auto &&part) {
    return std::string_view(std::addressof(*std::begin(part)), ranges::distance(part));
  };

  auto parts = static_vector<std::string_view, 8>{};
  for (auto &&part : input | ranges::views::split('.') | ranges::views::transform(to_string_view)) {
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

    auto number = details::parse_ipv4_number(std::string_view(part), validation_error);
    if (!number) {
      *validation_error |= true;
      return tl::make_unexpected(ipv4_address_errc::invalid_segment_number);
    }

    numbers.push_back(number.value());
  }

  constexpr auto greater_than_255 = [](auto number) { return number > 255; };

  if (ranges::cend(numbers) != ranges::find_if(numbers, greater_than_255)) {
    *validation_error |= true;
  }

  auto numbers_last_but_one = ranges::cend(numbers);
  --numbers_last_but_one;
  if (numbers_last_but_one != ranges::find_if(numbers | ranges::views::drop_last(1), greater_than_255)) {
    *validation_error |= true;
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
}  // namespace skyr::inline v2

#endif  // SKYR_V2_NETWORK_IPV4_ADDRESS_HPP
