// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_NETWORK_IPV6_ADDRESS_HPP
#define SKYR_V1_NETWORK_IPV6_ADDRESS_HPP

#include <string>
#include <string_view>
#include <array>
#include <optional>
#include <algorithm>
#include <iterator>
#include <tl/expected.hpp>
#include <skyr/v1/platform/endianness.hpp>

namespace skyr {
inline namespace v1 {
/// Enumerates IPv6 address parsing errors
enum class ipv6_address_errc {
  /// IPv6 address does not start with a double colon
  does_not_start_with_double_colon,
  /// IPv6 piece is not valid
  invalid_piece,
  /// IPv6 piece is not valid because address is expected to be
  /// compressed
  compress_expected,
  /// IPv4 segment is empty
  empty_ipv4_segment,
  /// IPv4 segment number is invalid
  invalid_ipv4_segment_number,
};

/// Represents an IPv6 address
class ipv6_address {

  std::array<unsigned short, 8> address_ = {0, 0, 0, 0, 0, 0, 0, 0};

 public:

  /// Constructor
  ipv6_address() = default;

  /// Constructor
  /// \param address Sets the IPv6 address to `address`
  explicit ipv6_address(std::array<unsigned short, 8> address) {
    constexpr static auto network_byte_order = [] (auto v) { return to_network_byte_order<unsigned short>(v); };

    std::transform(
        begin(address), end(address),
        begin(address_),
        network_byte_order);
  }

  /// The address in bytes in network byte order
  /// \returns The address in bytes
  [[nodiscard]] auto to_bytes() const noexcept -> std::array<unsigned char, 16> {
    std::array<unsigned char, 16> bytes{};
    for (auto i = 0UL; i < address_.size(); ++i) {
      bytes[i * 2    ] = static_cast<unsigned char>(address_[i] >> 8u); // NOLINT
      bytes[i * 2 + 1] = static_cast<unsigned char>(address_[i]); // NOLINT
    }
    return bytes;
  }

   /// \returns The IPv6 address as a string
  [[nodiscard]] auto serialize() const -> std::string;
};

/// Parses an IPv6 address
/// \param input An input string
/// \returns An `ipv6_address` object or an error
auto parse_ipv6_address(
    std::string_view input, bool *validation_error) -> tl::expected<ipv6_address, ipv6_address_errc>;
}  // namespace v1
}  // namespace skyr

#endif //SKYR_V1_NETWORK_IPV6_ADDRESS_HPP
