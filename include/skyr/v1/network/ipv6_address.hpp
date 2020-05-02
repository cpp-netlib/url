// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_NETWORK_IPV6_ADDRESS_HPP
#define SKYR_V1_NETWORK_IPV6_ADDRESS_HPP

#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <optional>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <system_error>
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

namespace details {
class ipv6_address_error_category : public std::error_category {
 public:
  [[nodiscard]] auto name() const noexcept -> const char * override {
    return "ipv6 address";
  }

  [[nodiscard]] auto message(int error) const noexcept -> std::string override {
    switch (static_cast<ipv6_address_errc>(error)) {
      case ipv6_address_errc::does_not_start_with_double_colon:
        return "IPv6 piece does not start with a double colon.";
      case ipv6_address_errc::invalid_piece:
        return "Invalid IPv6 piece.";
      case ipv6_address_errc::compress_expected:
        return "IPv6 address compression was expected.";
      case ipv6_address_errc::empty_ipv4_segment:
        return "IPv4 segment is empty.";
      case ipv6_address_errc::invalid_ipv4_segment_number:
        return "IPv4 segment number is invalid.";
      default:
        return "(Unknown error)";
    }
  }
};
}  // namespace details

/// Creates a `std::error_code` given a `skyr::ipv6_address_errc`
/// value
/// \param error An IPv6 address error
/// \returns A `std::error_code` object
inline auto make_error_code(ipv6_address_errc error) -> std::error_code {
  static const details::ipv6_address_error_category category{};
  return std::error_code(static_cast<int>(error), category);
}

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
  [[nodiscard]] auto serialize() const -> std::string {
    using namespace std::string_literals;

    auto output = ""s;
    auto compress = std::optional<size_t>();

    auto sequences = std::vector<std::pair<size_t, size_t>>();
    auto in_sequence = false;

    auto first = std::begin(address_), last = std::end(address_);
    auto it = first;
    while (true) {
      if (*it == 0) {
        auto index = std::distance(first, it);

        if (!in_sequence) {
          sequences.emplace_back(index, 1);
          in_sequence = true;
        } else {
          ++sequences.back().second;
        }
      } else {
        if (in_sequence) {
          if (sequences.back().second == 1) {
            sequences.pop_back();
          }
          in_sequence = false;
        }
      }
      ++it;

      if (it == last) {
        if (!sequences.empty() && (sequences.back().second == 1)) {
          sequences.pop_back();
        }
        in_sequence = false;
        break;
      }
    }

    if (!sequences.empty()) {
      constexpr static auto greater = [](const auto &lhs, const auto &rhs) -> bool { return lhs.second > rhs.second; };

      std::stable_sort(std::begin(sequences), std::end(sequences), greater);
      compress = sequences.front().first;
    }

    auto ignore0 = false;
    for (auto i = 0UL; i <= 7UL; ++i) {
      if (ignore0 && (address_[i] == 0)) { // NOLINT
        continue;
      } else if (ignore0) {
        ignore0 = false;
      }

      if (compress && (compress.value() == i)) {
        auto separator = (i == 0) ? "::"s : ":"s;
        output += separator;
        ignore0 = true;
        continue;
      }

      std::ostringstream oss;
      oss << std::hex << address_[i]; // NOLINT
      output += oss.str();

      if (i != 7) {
        output += ":";
      }
    }

    return output;
  }
};

/// Parses an IPv6 address
/// \param input An input string
/// \returns An `ipv6_address` object or an error
auto parse_ipv6_address(
    std::string_view input) -> tl::expected<ipv6_address, std::error_code>;
}  // namespace v1
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::v1::ipv6_address_errc> : true_type {};
}  // namespace std

#endif //SKYR_V1_NETWORK_IPV6_ADDRESS_HPP
