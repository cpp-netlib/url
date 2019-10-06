// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_IPV4_ADDRESS_INC
#define SKYR_IPV4_ADDRESS_INC

#include <string>
#include <string_view>
#include <system_error>
#include <optional>
#include <tl/expected.hpp>

namespace skyr {
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

/// Creates a `std::error_code` given a `skyr::ipv4_address_errc`
/// value
/// \param error An IPv4 address error
/// \returns A `std::error_code` object
std::error_code make_error_code(ipv4_address_errc error);

/// Represents an IPv4 address
class ipv4_address {

  unsigned int address_ = 0;

 public:

  /// Constructor
  ipv4_address() = default;

   /// Constructor
   /// \param address Sets the IPv4 address to `address`
  explicit ipv4_address(unsigned int address)
      : address_(address) {}

  /// The address value
  /// \returns The address value
  [[nodiscard]] unsigned int address() const noexcept {
    return address_;
  }

  /// \returns The address as a string
  [[nodiscard]] std::string to_string() const;

};

/// Parses an IPv4 address
/// \param input An input string
/// \returns An `ipv4_address` object or an error
tl::expected<ipv4_address, std::error_code> parse_ipv4_address(
    std::string_view input);
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::ipv4_address_errc> : true_type {};
}  // namespace std

#endif //SKYR_IPV4_ADDRESS_INC
