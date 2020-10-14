// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <locale>
#include <cassert>
#include <range/v3/algorithm/stable_sort.hpp>
#include <skyr/v1/network/ipv6_address.hpp>
#include <skyr/v1/string/starts_with.hpp>
#include <skyr/v1/containers/static_vector.hpp>

namespace skyr { inline namespace v1 {
[[nodiscard]] auto ipv6_address::serialize() const -> std::string {
  using namespace std::string_literals;

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

    auto buffer = std::array<char, 8>{};
    auto chars = std::snprintf(buffer.data(), buffer.size(), "%0x", address_[i]); // NOLINT
    std::copy(buffer.data(), buffer.data() + chars, std::back_inserter(output)); // NOLINT

    if (i != 7) {
      output += ":";
    }
  }

  return output;
}

namespace {
template <class intT, class charT>
auto hex_to_dec(charT byte) noexcept {
  assert(std::isxdigit(byte, std::locale::classic()));

  if (std::isdigit(byte, std::locale::classic())) {
    return static_cast<intT>(byte - '0');
  }

  return static_cast<intT>(std::tolower(byte, std::locale::classic()) - 'a' + 10);
}
} // namespace

auto parse_ipv6_address(
    std::string_view input, bool *validation_error) -> tl::expected<ipv6_address, ipv6_address_errc> {
  using namespace std::string_view_literals;

  auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 0}};
  auto piece_index = 0;
  auto compress = std::optional<decltype(piece_index)>();

  auto first = std::cbegin(input), last = std::cend(input);
  auto it = first;

  if (starts_with(input, "::"sv)) {
    ranges::advance(it, 2);
    ++piece_index;
    compress = piece_index;
  }
  else if (input.empty() || starts_with(input, ":"sv)) {
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

    while ((it != last) &&
           ((length < 4) && std::isxdigit(*it, std::locale::classic()))) {
      value = value * 0x10 + hex_to_dec<decltype(value)>(*it);
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

        address[piece_index] = static_cast<std::uint16_t>(address[piece_index] * 0x100 + ipv4_piece.value()); // NOLINT
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
    address[piece_index] = static_cast<unsigned short>(value); // NOLINT
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
    *validation_error |= true;
    return tl::make_unexpected(ipv6_address_errc::compress_expected);
  }

  return ipv6_address(address);
}
}  // namespace v1
}  // namespace skyr
