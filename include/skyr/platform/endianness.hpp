// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_PLATFORM_ENDIANNESS_HPP
#define SKYR_PLATFORM_ENDIANNESS_HPP

#include <array>
#include <bit>
#include <type_traits>

namespace skyr {
namespace details {
template <class IntT>
  requires std::is_integral_v<IntT>
constexpr auto swap_endianness(IntT v) noexcept -> IntT {
  constexpr auto byte_count = sizeof(v);
  constexpr auto bit_count = 8ul;
  std::array<std::byte, byte_count> bytes{};
  for (auto i = 0ul; i < byte_count; ++i) {
    bytes[i] = static_cast<std::byte>(v >> (i * bit_count));
  }
  IntT result = 0;
  for (auto i = 0ul; i < byte_count; ++i) {
    result |= (static_cast<IntT>(bytes[byte_count - 1 - i]) << (i * bit_count));
  }
  return result;
}
}  // namespace details

template <class IntT>
  requires std::is_integral_v<IntT>
constexpr auto to_network_byte_order(IntT v) noexcept -> IntT {
  return (std::endian::big == std::endian::native) ? v : details::swap_endianness(v);  // NOLINT
}

template <class IntT>
  requires std::is_integral_v<IntT>
constexpr auto from_network_byte_order(IntT v) noexcept -> IntT {
  return (std::endian::big == std::endian::native) ? v : details::swap_endianness(v);  // NOLINT
}
}  // namespace skyr

#endif  // SKYR_PLATFORM_ENDIANNESS_HPP
