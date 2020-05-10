// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_PLATFORM_ENDIANNESS_HPP
#define SKYR_V1_PLATFORM_ENDIANNESS_HPP

#include <type_traits>
#include <array>

namespace skyr {
inline namespace v1 {
inline auto is_big_endian() noexcept {
  const auto word = 0x0001;
  auto bytes = static_cast<const std::byte *>(static_cast<const void *>(&word));
  return bytes[0] != std::byte(0x01); // NOLINT
}

namespace details {
template <typename intT>
inline auto swap_endianness(
    intT v, typename std::enable_if<std::is_integral<intT>::value>::type * = nullptr) noexcept -> intT {
  constexpr auto byte_count = sizeof(v);
  std::array<std::byte, byte_count> bytes{};
  for (auto i = 0UL; i < byte_count; ++i) {
    bytes[i] = static_cast<std::byte>(v >> (i * 8));
  }
  return *static_cast<const intT *>(static_cast<const void *>(bytes.data()));
}
}  // namespace details

template <class intT>
inline auto to_network_byte_order(intT v) noexcept {
  return (is_big_endian()) ? v : details::swap_endianness(v);
}

template <class intT>
inline auto from_network_byte_order(intT v) noexcept {
  return (is_big_endian()) ? v : details::swap_endianness(v);
}
}  // namespace v1
}  // namespace skyr

#endif // SKYR_V1_PLATFORM_ENDIANNESS_HPP
