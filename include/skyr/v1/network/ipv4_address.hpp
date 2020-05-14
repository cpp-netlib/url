// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_NETWORK_IPV4_ADDRESS_HPP
#define SKYR_V1_NETWORK_IPV4_ADDRESS_HPP

#include <array>
#include <string>
#include <string_view>
#include <system_error>
#include <optional>
#include <cmath>
#include <cerrno>
#include <locale>
#include <vector>
#include <tl/expected.hpp>
#include <skyr/v1/platform/endianness.hpp>

namespace skyr {
inline namespace v1 {
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

namespace details {
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
}  // namespace details

/// Creates a `std::error_code` given a `skyr::ipv4_address_errc`
/// value
/// \param error An IPv4 address error
/// \returns A `std::error_code` object
inline auto make_error_code(ipv4_address_errc error) -> std::error_code {
  static const details::ipv4_address_error_category category{};
  return std::error_code(static_cast<int>(error), category);
}

/// Represents an IPv4 address
class ipv4_address {

  unsigned int address_ = 0;

 public:

  /// Constructor
  ipv4_address() = default;

   /// Constructor
   /// \param address Sets the IPv4 address to `address`
  explicit ipv4_address(unsigned int address)
      : address_(to_network_byte_order(address)) {}

  /// The address value
  /// \returns The address value
  [[nodiscard]] auto address() const noexcept {
    return from_network_byte_order(address_);
  }

  /// The address in bytes in network byte order
  /// \returns The address in bytes
  [[nodiscard]] auto to_bytes() const noexcept -> std::array<unsigned char, 4> {
    return {{
      static_cast<unsigned char>(address_ >> 24u),
      static_cast<unsigned char>(address_ >> 16u),
      static_cast<unsigned char>(address_ >>  8u),
      static_cast<unsigned char>(address_)
    }};
  }

  /// \returns The address as a string
  [[nodiscard]] auto serialize() const
      -> std::string {
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
};

namespace details {
inline auto parse_ipv4_number(
    std::string_view input,
    bool *validation_error) -> tl::expected<std::uint64_t, std::error_code> {
  auto base = 10;

  if (
      (input.size() >= 2) && (input[0] == '0') &&
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
  if (errno == ERANGE || (pos != input.data() + input.size())) {
    return tl::make_unexpected(
        make_error_code(ipv4_address_errc::invalid_segment_number));
  }
  return number;
}
}  // namespace details

/// Parses an IPv4 address
/// \param input An input string
/// \returns An `ipv4_address` object or an error
inline auto parse_ipv4_address(
    std::string_view input, bool *validation_error) -> tl::expected<ipv4_address, std::error_code> {
  using namespace std::string_view_literals;

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
    *validation_error |= true;
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

    auto number = details::parse_ipv4_number(std::string_view(part), validation_error);
    if (!number) {
      *validation_error |= true;
      return tl::make_unexpected(make_error_code(ipv4_address_errc::invalid_segment_number));
    }

    numbers.push_back(number.value());
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

namespace std {
template <>
struct is_error_code_enum<skyr::v1::ipv4_address_errc> : true_type {};
}  // namespace std

#endif //SKYR_V1_NETWORK_IPV4_ADDRESS_HPP
