// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <locale>
#include <skyr/v1/network/ipv6_address.hpp>
#include "v1/string/starts_with.hpp"

namespace skyr {
inline namespace v1 {
using namespace std::string_view_literals;

namespace {
inline auto hex_to_dec(char byte) noexcept {
  assert(std::isxdigit(byte, std::locale::classic()));

  auto byte_lower = std::tolower(byte, std::locale::classic());

  if (std::isdigit(byte_lower, std::locale::classic())) {
    return static_cast<std::uint16_t>(byte_lower - '0');
  }

  return static_cast<std::uint16_t>(byte_lower - 'a' + 10);
}
}  // namespace

namespace details {
namespace {
auto parse_ipv6_address(std::string_view input) -> std::pair<tl::expected<ipv6_address, std::error_code>, bool> {
  auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 0}};
  auto piece_index = 0;
  auto compress = std::optional<decltype(piece_index)>();

  auto first = begin(input), last = end(input);
  auto it = first;

  if (it == last) {
    return
        std::make_pair(
            tl::make_unexpected(
                make_error_code(
                    ipv6_address_errc::does_not_start_with_double_colon)), true);
  }

  if (*it == ':') {
    auto next_it = it;
    ++next_it;
    if (!starts_with(input.substr(std::distance(std::begin(input), next_it)), ":"sv)) {
      return
          std::make_pair(
              tl::make_unexpected(
                  make_error_code(
                      ipv6_address_errc::does_not_start_with_double_colon)), true);
    }

    std::advance(it, 2);
    ++piece_index;
    compress = piece_index;
  }

  while (it != last) {
    if (piece_index == 8) {
      return
          std::make_pair(
              tl::make_unexpected(
                  make_error_code(ipv6_address_errc::invalid_piece)), true);
    }

    if (*it == ':') {
      if (compress) {
        return
            std::make_pair(
                tl::make_unexpected(
                    make_error_code(ipv6_address_errc::compress_expected)), true);
      }

      ++it;
      ++piece_index;
      compress = piece_index;
      continue;
    }

    auto value = 0;
    auto length = 0;

    while (
        (it != last) &&
            ((length < 4) && std::isxdigit(*it, std::locale::classic()))) {
      value = value * 0x10 + hex_to_dec(*it);
      ++it;
      ++length;
    }

    if ((it != last) && (*it == '.')) {
      if (length == 0) {
        return
            std::make_pair(
                tl::make_unexpected(
                    make_error_code(ipv6_address_errc::empty_ipv4_segment)), true);
      }

      std::advance(it, -length);

      if (piece_index > 6) {
        return
            std::make_pair(
                tl::make_unexpected(
                    make_error_code(
                        ipv6_address_errc::invalid_ipv4_segment_number)), true);
      }

      auto numbers_seen = 0;

      while (it != last) {
        auto ipv4_piece = std::optional<std::uint16_t>();

        if (numbers_seen > 0) {
          if ((*it == '.') && (numbers_seen < 4)) {
            ++it;
          } else {
            return
                std::make_pair(
                    tl::make_unexpected(
                        make_error_code(
                            ipv6_address_errc::invalid_ipv4_segment_number)), true);
          }
        }

        if ((it == last) || !std::isdigit(*it, std::locale::classic())) {
          return
              std::make_pair(
                  tl::make_unexpected(
                      make_error_code(
                          ipv6_address_errc::invalid_ipv4_segment_number)), true);
        }

        while ((it != last) && std::isdigit(*it, std::locale::classic())) {
          auto number = static_cast<std::uint16_t>(*it - '0');
          if (!ipv4_piece) {
            ipv4_piece = number;
          } else if (ipv4_piece.value() == 0) {
            return
                std::make_pair(
                    tl::make_unexpected(
                        make_error_code(
                            ipv6_address_errc::invalid_ipv4_segment_number)), true);
          } else {
            ipv4_piece = ipv4_piece.value() * std::uint16_t(10) + number;
          }

          if (ipv4_piece.value() > 255) {
            return
                std::make_pair(
                    tl::make_unexpected(
                        make_error_code(
                            ipv6_address_errc::invalid_ipv4_segment_number)), true);
          }

          ++it;
        }

        address[piece_index] = address[piece_index] * 0x100 + ipv4_piece.value(); // NOLINT
        ++numbers_seen;

        if ((numbers_seen == 2) || (numbers_seen == 4)) {
          ++piece_index;
        }
      }

      if (numbers_seen != 4) {
        return
            std::make_pair(
                tl::make_unexpected(
                    make_error_code(
                        ipv6_address_errc::invalid_ipv4_segment_number)), true);
      }

      break;
    } else if ((it != last) && (*it == ':')) {
      ++it;
      if (it == last) {
        return
            std::make_pair(
                tl::make_unexpected(
                    make_error_code(ipv6_address_errc::invalid_piece)), true);
      }
    } else if (it != last) {
      return
          std::make_pair(
              tl::make_unexpected(
                  make_error_code(ipv6_address_errc::invalid_piece)), true);
    }
    address[piece_index] = value; // NOLINT
    ++piece_index;
  }

  if (compress) {
    auto swaps = piece_index - compress.value();
    piece_index = 7;
    while ((piece_index != 0) && (swaps > 0)) {
      std::swap(address[piece_index], address[compress.value() + swaps - 1]); // NOLINT
      --piece_index;
      --swaps;
    }
  } else if (!compress && (piece_index != 8)) {
    return
        std::make_pair(
            tl::make_unexpected(
                make_error_code(ipv6_address_errc::compress_expected)), true);
  }

  return std::make_pair(ipv6_address(address), false);
}
}  // namespace
}  // namespace details

auto parse_ipv6_address(std::string_view input) -> tl::expected<ipv6_address, std::error_code> {
  return details::parse_ipv6_address(input).first;
}
}  // namespace v1
}  // namespace skyr
