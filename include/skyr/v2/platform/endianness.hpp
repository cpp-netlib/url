// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_PLATFORM_ENDIANNESS_HPP
#define SKYR_V2_PLATFORM_ENDIANNESS_HPP

#include <bit>
#include <type_traits>
#include <array>

namespace skyr::inline v2 {
namespace details {
template <class intT>
requires std::is_integral_v<intT> constexpr inline auto swap_endianness(intT v) noexcept -> intT {
  constexpr auto byte_count = sizeof(v);
  std::array<unsigned char, byte_count> bytes{};
  for (auto i = 0UL; i < byte_count; ++i) {
    bytes[i] = static_cast<unsigned char>(v >> (i * 8));
  }
  return *static_cast<const intT *>(static_cast<const void *>(bytes.data()));
}
}  // namespace details

template <class intT>
requires std::is_integral_v<intT> constexpr inline auto to_network_byte_order(intT v) noexcept {
  return (std::endian::big == std::endian::native) ? v : details::swap_endianness(v);  // NOLINT
}

template <class intT>
requires std::is_integral_v<intT> constexpr inline auto from_network_byte_order(intT v) noexcept {
  return (std::endian::big == std::endian::native) ? v : details::swap_endianness(v);  // NOLINT
}
}  // namespace skyr::inline v2

#endif  // SKYR_V2_PLATFORM_ENDIANNESS_HPP
