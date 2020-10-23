// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_NETWORK_IPV6_ADDRESS_HPP
#define SKYR_V2_NETWORK_IPV6_ADDRESS_HPP

#include <string>
#include <string_view>
#include <array>
#include <optional>
#include <algorithm>
#include <iterator>
#include <locale>
#include <tl/expected.hpp>
#include <range/v3/algorithm/stable_sort.hpp>
#include <skyr/v2/containers/static_vector.hpp>
#include <skyr/v2/platform/endianness.hpp>
#include <fmt/format.h>

namespace skyr::inline v2 {
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
  constexpr ipv6_address() = default;

  /// Constructor
  /// \param address Sets the IPv6 address to `address`
  constexpr explicit ipv6_address(std::array<unsigned short, 8> address) {
    constexpr auto network_byte_order = [](auto v) { return to_network_byte_order<unsigned short>(v); };

    std::transform(begin(address), end(address), begin(address_), network_byte_order);
  }

  /// The address in bytes in network byte order
  /// \returns The address in bytes
  [[nodiscard]] constexpr auto to_bytes() const noexcept -> std::array<unsigned char, 16> {
    std::array<unsigned char, 16> bytes{};
    for (auto i = 0UL; i < address_.size(); ++i) {
      bytes[i * 2] = static_cast<unsigned char>(address_[i] >> 8u);  // NOLINT
      bytes[i * 2 + 1] = static_cast<unsigned char>(address_[i]);    // NOLINT
    }
    return bytes;
  }

  /// \returns The IPv6 address as a string
  [[nodiscard]] auto serialize() const -> std::string {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    auto output = ""s;
    auto compress = std::optional<std::size_t>();

    auto sequences = static_vector<std::pair<std::size_t, std::size_t>, 8>{};
    auto in_sequence = false;

    auto first = std::cbegin(address_), last = std::cend(address_);
    auto it = first;
    while (true) {
      if (*it == 0) {
        auto index = ranges::distance(first, it);

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

      ranges::stable_sort(sequences, greater);
      compress = sequences.front().first;
    }

    auto ignore0 = false;
    for (auto i = 0UL; i <= 7UL; ++i) {
      if (ignore0 && (address_[i] == 0)) {  // NOLINT
        continue;
      } else if (ignore0) {
        ignore0 = false;
      }

      if (compress && (compress.value() == i)) {
        auto separator = (i == 0) ? "::"sv : ":"sv;
        output += separator;
        ignore0 = true;
        continue;
      }

      constexpr auto separator = [](auto i) { return (i != 7) ? ":"sv : ""sv; };

      output += fmt::format("{:x}{}", address_[i], separator(i));  // NOLINT
    }

    return output;
  }
};

namespace details {
template <class intT, class charT>
constexpr inline auto hex_to_dec(charT byte) noexcept {
  assert(std::isxdigit(byte, std::locale::classic()));

  if (std::isdigit(byte, std::locale::classic())) {
    return static_cast<intT>(byte - '0');
  }

  return static_cast<intT>(std::tolower(byte, std::locale::classic()) - 'a' + 10);
}
}  // namespace details

/// Parses an IPv6 address
/// \param input An input string
/// \returns An `ipv6_address` object or an error
constexpr inline auto parse_ipv6_address(std::string_view input, bool *validation_error)
    -> tl::expected<ipv6_address, ipv6_address_errc> {
  using namespace std::string_view_literals;

  auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 0}};
  auto piece_index = 0;
  auto compress = std::optional<decltype(piece_index)>();

  auto first = std::cbegin(input), last = std::cend(input);
  auto it = first;

  if (input.starts_with("::"sv)) {
    ranges::advance(it, 2);
    ++piece_index;
    compress = piece_index;
  } else if (input.empty() || input.starts_with(":"sv)) {
    *validation_error |= true;
    return tl::make_unexpected(ipv6_address_errc::does_not_start_with_double_colon);
  }

  while (it != last) {
    if (piece_index == 8) {
      *validation_error |= true;
      return tl::make_unexpected(ipv6_address_errc::invalid_piece);
    }

    if (*it == ':') {
      if (compress) {
        *validation_error |= true;
        return tl::make_unexpected(ipv6_address_errc::compress_expected);
      }

      ++it;
      ++piece_index;
      compress = piece_index;
      continue;
    }

    auto value = 0;
    auto length = 0;

    while ((it != last) && ((length < 4) && std::isxdigit(*it, std::locale::classic()))) {
      value = value * 0x10 + details::hex_to_dec<decltype(value)>(*it);
      ++it;
      ++length;
    }

    if ((it != last) && (*it == '.')) {
      if (length == 0) {
        *validation_error |= true;
        return tl::make_unexpected(ipv6_address_errc::empty_ipv4_segment);
      }

      ranges::advance(it, -length);

      if (piece_index > 6) {
        *validation_error |= true;
        return tl::make_unexpected(ipv6_address_errc::invalid_ipv4_segment_number);
      }

      auto numbers_seen = 0;

      while (it != last) {
        auto ipv4_piece = std::optional<int>();

        if (numbers_seen > 0) {
          if ((*it == '.') && (numbers_seen < 4)) {
            ++it;
          } else {
            *validation_error |= true;
            return tl::make_unexpected(ipv6_address_errc::invalid_ipv4_segment_number);
          }
        }

        if ((it == last) || !std::isdigit(*it, std::locale::classic())) {
          *validation_error |= true;
          return tl::make_unexpected(ipv6_address_errc::invalid_ipv4_segment_number);
        }

        while ((it != last) && std::isdigit(*it, std::locale::classic())) {
          auto number = *it - '0';
          if (!ipv4_piece) {
            ipv4_piece = number;
          } else if (ipv4_piece.value() == 0) {
            *validation_error |= true;
            return tl::make_unexpected(ipv6_address_errc::invalid_ipv4_segment_number);
          } else {
            ipv4_piece = ipv4_piece.value() * 10 + number;
          }

          if (ipv4_piece.value() > 255) {
            *validation_error |= true;
            return tl::make_unexpected(ipv6_address_errc::invalid_ipv4_segment_number);
          }

          ++it;
        }

        address[piece_index] = static_cast<std::uint16_t>((address[piece_index] << 8) + ipv4_piece.value());  // NOLINT
        ++numbers_seen;

        if ((numbers_seen == 2) || (numbers_seen == 4)) {
          ++piece_index;
        }
      }

      if (numbers_seen != 4) {
        *validation_error |= true;
        return tl::make_unexpected(ipv6_address_errc::invalid_ipv4_segment_number);
      }

      break;
    } else if ((it != last) && (*it == ':')) {
      ++it;
      if (it == last) {
        *validation_error |= true;
        return tl::make_unexpected(ipv6_address_errc::invalid_piece);
      }
    } else if (it != last) {
      *validation_error |= true;
      return tl::make_unexpected(ipv6_address_errc::invalid_piece);
    }
    address[piece_index] = static_cast<unsigned short>(value);  // NOLINT
    ++piece_index;
  }

  if (compress) {
    auto swaps = piece_index - compress.value();
    piece_index = 7;
    while ((piece_index != 0) && (swaps > 0)) {
      std::swap(address[piece_index], address[compress.value() + swaps - 1]);  // NOLINT
      --piece_index;
      --swaps;
    }
  } else if (!compress && (piece_index != 8)) {
    *validation_error |= true;
    return tl::make_unexpected(ipv6_address_errc::compress_expected);
  }

  return ipv6_address(address);
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_NETWORK_IPV6_ADDRESS_HPP
