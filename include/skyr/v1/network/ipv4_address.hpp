// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_NETWORK_IPV4_ADDRESS_HPP
#define SKYR_V1_NETWORK_IPV4_ADDRESS_HPP

#include <array>
#include <string>
#include <string_view>
#include <optional>
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
  [[nodiscard]] auto serialize() const -> std::string;
};

/// Parses an IPv4 address
/// \param input An input string
/// \returns An `ipv4_address` object or an error
auto parse_ipv4_address(
    std::string_view input, bool *validation_error) -> tl::expected<ipv4_address, ipv4_address_errc>;
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_NETWORK_IPV4_ADDRESS_HPP
