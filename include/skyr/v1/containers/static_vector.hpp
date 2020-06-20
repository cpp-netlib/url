// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V1_CONTAINERS_STATIC_VECTOR_HPP
#define SKYR_V1_CONTAINERS_STATIC_VECTOR_HPP

#include <cstdlib>
#include <array>
#include <type_traits>
#include <optional>
#include <tl/expected.hpp>

namespace skyr {
inline namespace v1 {
enum class static_vector_errc {
  invalid_value = 1,
  capacity_reached,
};

template <
    class T,
    std::size_t Capacity
    >
class static_vector {
 private:

  using impl_type = std::array<T, Capacity>;

  impl_type impl_;
  std::size_t size_ = 0;

 public:

  using value_type = T;
  using const_reference = const T &;
  using reference = T &;
  using const_pointer = const T *;
  using pointer = T *;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using const_iterator = typename impl_type::const_iterator;
  using iterator = typename impl_type::iterator;

  constexpr static_vector() noexcept(std::is_trivially_constructible_v<T>) = default;

  constexpr auto front() const noexcept -> const_reference {
    return impl_[0];
  }

  constexpr auto front() noexcept -> reference {
    return impl_[0];
  }

  constexpr auto back() const noexcept -> const_reference {
    return impl_[size_ - 1];
  }

  constexpr auto back() noexcept -> reference {
    return impl_[size_ - 1];
  }

  constexpr auto push_back(const_reference value)
    noexcept -> tl::expected<value_type, static_vector_errc> {
    if (size_ == Capacity) {
      return tl::make_unexpected(static_vector_errc::capacity_reached);
    }
    impl_[size_++] = value;
    return impl_[size_ - 1];
  }

  template <class... Args>
  constexpr auto emplace_back(Args &&... args)
    noexcept(std::is_trivially_move_assignable_v<T>) -> tl::expected<value_type, static_vector_errc> {
    if (size_ + sizeof...(Args) > Capacity) {
      return tl::make_unexpected(static_vector_errc::capacity_reached);
    }
    impl_[size_++] = value_type{args...};
    return impl_[size_ - 1];
  }

  constexpr void pop_back() noexcept {
    if (size_ != 0) {
      --size_;
    }
  }

  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return size_;
  }

  [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
    return Capacity;
  }

  [[nodiscard]] constexpr auto empty() const noexcept -> bool {
    return size_ == 0;
  }

  [[nodiscard]] constexpr auto full() const noexcept -> bool {
    return size_ == Capacity;
  }

  [[nodiscard]] constexpr auto begin() noexcept -> iterator {
    return impl_.begin();
  }

  [[nodiscard]] constexpr auto end() noexcept -> iterator {
    auto last = impl_.begin();
    std::advance(last, size_);
    return last;
  }

  [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
    return impl_.begin();
  }

  [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
    auto last = impl_.begin();
    std::advance(last, size_);
    return last;
  }

};
}  // namespace v1
}  // namespace skyr

#endif  // SKYR_V1_CONTAINERS_STATIC_VECTOR_HPP
