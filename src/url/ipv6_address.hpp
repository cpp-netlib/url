// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_IPV6_ADDRESS_INC
#define SKYR_IPV6_ADDRESS_INC

#include <string>
#include <string_view>
#include <array>
#include <algorithm>
#include <iterator>
#include <system_error>
#include <tl/expected.hpp>

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

/// Creates a `std::error_code` given a `skyr::ipv6_address_errc`
/// value
/// \param error An IPv6 address error
/// \returns A `std::error_code` object
std::error_code make_error_code(ipv6_address_errc error);

/// Represents an IPv6 address
class ipv6_address {

  std::array<unsigned short, 8> address_ = {0, 0, 0, 0, 0, 0, 0, 0};

  using repr_type = decltype(address_);

 public:

  /// Constructor
  ipv6_address() = default;

  /// Constructor
  /// \param address Sets the IPv6 address to `address`
  explicit ipv6_address(std::array<unsigned short, 8> address)
      : address_(address) {}

   /// \returns The IPv4 address as a string
  [[nodiscard]] std::string to_string() const;

};

/// Parses an IPv6 address
/// \param input An input string
/// \returns An `ipv6_address` object or an error
tl::expected<ipv6_address, std::error_code> parse_ipv6_address(
    std::string_view input);
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::ipv6_address_errc> : true_type {};
}  // namespace std

#endif //SKYR_IPV6_ADDRESS_INC
