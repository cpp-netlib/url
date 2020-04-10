// Copyright 2018-20 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <vector>
#include <cassert>
#include <cstring>
#include <locale>
#include <algorithm>
#include <optional>
#include <skyr/network/ipv6_address.hpp>
#include "string/starts_with.hpp"

namespace skyr {
inline namespace v1 {
using namespace std::string_view_literals;

namespace {
class ipv6_address_error_category : public std::error_category {
 public:
  [[nodiscard]] const char *name() const noexcept override;
  [[nodiscard]] std::string message(int error) const noexcept override;
};

const char *ipv6_address_error_category::name() const noexcept {
  return "ipv6 address";
}

std::string ipv6_address_error_category::message(int error) const noexcept {
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

const ipv6_address_error_category category{};
}  // namespace

std::error_code make_error_code(ipv6_address_errc error) {
  return std::error_code(static_cast<int>(error), category);
}

namespace {
inline std::uint16_t hex_to_dec(char byte) noexcept {
  assert(std::isxdigit(byte, std::locale::classic()));

  auto byte_lower = std::tolower(byte, std::locale::classic());

  if (std::isdigit(byte_lower, std::locale::classic())) {
    return static_cast<std::uint16_t>(byte_lower - '0');
  }

  return static_cast<std::uint16_t>(byte_lower - 'a') + 10;
}
}  // namespace

std::string ipv6_address::to_string() const {
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
    stable_sort(std::begin(sequences), std::end(sequences),
                [](const auto &lhs,
                   const auto &rhs) -> bool {
                  return lhs.second > rhs.second;
                });
    compress = sequences.front().first;
  }

  auto ignore0 = false;
  for (auto i = 0UL; i <= 7UL; ++i) {
    if (ignore0 && (address_[i] == 0)) {
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
    oss << std::hex << address_[i];
    output += oss.str();

    if (i != 7) {
      output += ":";
    }
  }

  return output;
}

namespace details {
namespace {
std::pair<tl::expected<ipv6_address, std::error_code>, bool> parse_ipv6_address(std::string_view input) {
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
    if (!starts_with(std::string_view(std::addressof(*next_it), std::distance(next_it, last)), ":"sv)) {
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

        address[piece_index] = address[piece_index] * 0x100 + ipv4_piece.value();
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
    address[piece_index] = value;
    ++piece_index;
  }

  if (compress) {
    auto swaps = piece_index - compress.value();
    piece_index = 7;
    while ((piece_index != 0) && (swaps > 0)) {
      std::swap(address[piece_index], address[compress.value() + swaps - 1]);
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


tl::expected<ipv6_address, std::error_code> parse_ipv6_address(std::string_view input) {
  return details::parse_ipv6_address(input).first;
}
}  // namespace v1
}  // namespace skyr
