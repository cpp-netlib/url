// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_PLATFORM_ENDIANNESS_HPP
#define SKYR_PLATFORM_ENDIANNESS_HPP

#include <bit>
#include <type_traits>
#include <array>

namespace skyr {
namespace details {
template <class intT>
requires std::is_integral_v<intT> constexpr inline auto swap_endianness(intT v) noexcept -> intT {
  constexpr auto byte_count = sizeof(v);
  constexpr auto bit_count = 8ul;
  std::array<std::byte, byte_count> bytes{};
  for (auto i = 0ul; i < byte_count; ++i) {
    bytes[i] = static_cast<std::byte>(v >> (i * bit_count));
  }
  intT result = 0;
  for (auto i = 0ul; i < byte_count; ++i) {
    result |= (static_cast<intT>(bytes[byte_count - 1 - i]) << (i * bit_count));
  }
  return result;
}
}  // namespace details

template <class intT>
requires std::is_integral_v<intT> constexpr inline auto to_network_byte_order(intT v) noexcept -> intT {
  return (std::endian::big == std::endian::native) ? v : details::swap_endianness(v);  // NOLINT
}

template <class intT>
requires std::is_integral_v<intT> constexpr inline auto from_network_byte_order(intT v) noexcept -> intT {
  return (std::endian::big == std::endian::native) ? v : details::swap_endianness(v);  // NOLINT
}
}  // namespace skyr

#endif  // SKYR_PLATFORM_ENDIANNESS_HPP
