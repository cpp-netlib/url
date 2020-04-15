// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_IPV4_ADDRESS_INC
#define SKYR_IPV4_ADDRESS_INC

#include <array>
#include <string>
#include <string_view>
#include <system_error>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/platform/endianness.hpp>

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
  [[nodiscard]] auto to_string() const -> std::string;

};

/// Parses an IPv4 address
/// \param input An input string
/// \returns An `ipv4_address` object or an error
auto parse_ipv4_address(
    std::string_view input) -> tl::expected<ipv4_address, std::error_code>;
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::ipv4_address_errc> : true_type {};
}  // namespace std

#endif //SKYR_IPV4_ADDRESS_INC
