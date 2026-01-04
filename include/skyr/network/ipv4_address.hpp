// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_NETWORK_IPV4_ADDRESS_HPP
#define SKYR_NETWORK_IPV4_ADDRESS_HPP

#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <cstdint>
#include <expected>
#include <format>
#include <locale>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>

#include <skyr/containers/static_vector.hpp>
#include <skyr/platform/endianness.hpp>

namespace skyr {
/// Enumerates IPv4 address parsing errors
enum class ipv4_address_errc {
  /// The input contains more than 4 segments
  too_many_segments,
  /// The input contains an empty segment
  empty_segment,
  /// The segment numbers are invalid
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
    const auto addr = address();
    return {{static_cast<unsigned char>(addr >> 24u),
             static_cast<unsigned char>(addr >> 16u),  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
             static_cast<unsigned char>(addr >> 8u),
             static_cast<unsigned char>(addr)}};  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
  }

  /// \returns The address as a string
  [[nodiscard]] auto serialize() const -> std::string {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    constexpr auto separator = [](auto i) {
      return (i < 4) ? "."sv : ""sv;
    };  // NOLINT(cppcoreguidelines-avoid-magic-numbers)

    auto output = ""s;
    auto n = address();
    for (auto i = 1U; i <= 4U; ++i) {
      output = std::format("{}{}{}", separator(i), n % 256, output);
      n >>= 8;  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    }
    return output;
  }
};

namespace details {
/// Computes 256^exp efficiently using bit shifts (256 = 2^8, so 256^n = 2^(8n))
constexpr auto pow256(unsigned int exp) noexcept -> std::uint64_t {
  return 1ULL << (exp * 8);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
}

constexpr auto parse_ipv4_number(std::string_view input, bool* validation_error)
    -> std::expected<std::uint64_t, ipv4_address_errc> {
  auto base = 10;  // NOLINT(cppcoreguidelines-avoid-magic-numbers)

  if ((input.size() >= 2) && (input[0] == '0') && (std::tolower(input[1], std::locale::classic()) == 'x')) {
    *validation_error |= true;
    input = input.substr(2);
    base = 16;  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
  } else if ((input.size() >= 2) && (input[0] == '0')) {
    *validation_error |= true;
    input = input.substr(1);
    base = 8;  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
  }

  if (input.empty()) {
    return 0ULL;
  }

  char* pos = const_cast<char*>(input.data()) + input.size();  // NOLINT
  auto number = std::strtoull(input.data(), &pos, base);
  if ((number == ULLONG_MAX) || (pos != input.data() + input.size())) {
    return std::unexpected(ipv4_address_errc::invalid_segment_number);
  }
  return number;
}
}  // namespace details

/// Parses an IPv4 address
/// \param input An input string
/// \param validation_error Optional pointer to a bool that will be set if a validation error occurs
/// \returns An `ipv4_address` object or an error
constexpr auto parse_ipv4_address(std::string_view input, bool* validation_error)
    -> std::expected<ipv4_address, ipv4_address_errc> {
  using namespace std::string_view_literals;

  constexpr auto to_string_view = [](auto&& part) {
    return std::string_view(std::addressof(*std::begin(part)), std::ranges::distance(part));
  };

  auto parts = static_vector<std::string_view, 8>{};
  for (auto&& part : input | std::ranges::views::split('.') | std::ranges::views::transform(to_string_view)) {
    if (parts.size() == parts.max_size()) {
      *validation_error |= true;
      return std::unexpected(ipv4_address_errc::too_many_segments);
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
    return std::unexpected(ipv4_address_errc::too_many_segments);
  }

  auto numbers = static_vector<std::uint64_t, 4>{};

  for (const auto& part : parts) {
    if (part.empty()) {
      *validation_error |= true;
      return std::unexpected(ipv4_address_errc::empty_segment);
    }

    auto number = details::parse_ipv4_number(std::string_view(part), validation_error);
    if (!number) {
      *validation_error |= true;
      return std::unexpected(ipv4_address_errc::invalid_segment_number);
    }

    numbers.push_back(number.value());
  }

  constexpr auto greater_than_255 = [](auto number) {
    return number > 255;
  };  // NOLINT(cppcoreguidelines-avoid-magic-numbers)

  if (std::ranges::cend(numbers) != std::ranges::find_if(numbers, greater_than_255)) {
    *validation_error |= true;
  }

  // Check all but the last number don't exceed 255
  auto numbers_last = std::ranges::cend(numbers);
  --numbers_last;
  if (numbers_last != std::ranges::find_if(std::ranges::cbegin(numbers), numbers_last, greater_than_255)) {
    *validation_error |= true;
    return std::unexpected(ipv4_address_errc::overflow);
  }

  if (numbers.back() >= details::pow256(5 - numbers.size())) {  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    *validation_error |= true;
    return std::unexpected(ipv4_address_errc::overflow);
  }

  auto ipv4 = numbers.back();
  numbers.pop_back();

  auto counter = 0UL;
  for (auto&& number : numbers) {
    ipv4 += number * details::pow256(3 - counter);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ++counter;
  }
  return ipv4_address(static_cast<unsigned int>(ipv4));
}
}  // namespace skyr

#endif  // SKYR_NETWORK_IPV4_ADDRESS_HPP
