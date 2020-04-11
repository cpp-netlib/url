// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_ENDIANNESS_HPP
#define SKYR_URL_ENDIANNESS_HPP

#include <type_traits>
#include <array>

namespace skyr {
inline namespace v1 {
namespace details {
inline bool is_big_endian() noexcept {
  const auto word = 0x0001;
  auto bytes = static_cast<const unsigned char *>(static_cast<const void *>(&word));
  return bytes[0] != 0x01;
}

template <typename intT>
inline intT swap_endianness(intT v, typename std::enable_if<std::is_integral<intT>::value>::type * = nullptr) noexcept {
  constexpr auto byte_count = sizeof(v);
  std::array<unsigned char, byte_count> bytes{};
  for (auto i = 0UL; i < byte_count; ++i) {
    bytes[i] = (v >> (i * 8));
  }
  return *static_cast<const intT *>(static_cast<const void *>(bytes.data()));
}

inline unsigned int to_network_byte_order(unsigned int v) noexcept {
  return (is_big_endian()) ? v : swap_endianness(v);
}

inline unsigned int from_network_byte_order(unsigned int v) noexcept {
  return (is_big_endian()) ? v : swap_endianness(v);
}
}  // namespace details
}  // namespace v1
}  // namespace skyr

#endif //SKYR_URL_ENDIANNESS_HPP
