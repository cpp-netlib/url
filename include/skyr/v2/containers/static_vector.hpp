// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_V2_CONTAINERS_STATIC_VECTOR_HPP
#define SKYR_V2_CONTAINERS_STATIC_VECTOR_HPP

#include <cstdlib>
#include <array>
#include <type_traits>
#include <optional>
#include <cassert>

namespace skyr::inline v2 {
///
/// \tparam T
/// \tparam Capacity
template <class T, std::size_t Capacity>
class static_vector {
 private:
  using impl_type = std::array<T, Capacity>;

  impl_type impl_;
  std::size_t size_ = 0;

 public:
  ///
  using value_type = T;
  ///
  using const_reference = const T &;
  ///
  using reference = T &;
  ///
  using const_pointer = const T *;
  ///
  using pointer = T *;
  ///
  using size_type = std::size_t;
  ///
  using difference_type = std::ptrdiff_t;
  ///
  using const_iterator = typename impl_type::const_iterator;
  ///
  using iterator = typename impl_type::iterator;

  /// Constructor
  constexpr static_vector() = default;

  constexpr static_vector(const static_vector &) = default;
  constexpr static_vector(static_vector &&) noexcept = default;
  constexpr auto operator=(const static_vector &) -> static_vector & = default;
  constexpr auto operator=(static_vector &&) noexcept -> static_vector & = default;

  constexpr ~static_vector() {
    clear();
  }

  /// Gets the first const element in the vector
  /// \return a const T &
  /// \pre `size_ > 0`
  constexpr auto front() const noexcept -> const_reference {
    assert(size() > 0);
    return impl_[0];
  }

  /// Gets the first element in the vector
  /// \return a T &
  /// \pre `size() > 0`
  constexpr auto front() noexcept -> reference {
    assert(size_ > 0);
    return impl_[0];
  }

  ///
  /// \return
  /// \pre `size() > 0`
  constexpr auto back() const noexcept -> const_reference {
    assert(size_ > 0);
    return impl_[size_ - 1];
  }

  ///
  /// \return
  /// \pre `size() > 0`
  constexpr auto back() noexcept -> reference {
    assert(size_ > 0);
    return impl_[size_ - 1];
  }

  ///
  /// \param value
  /// \return
  /// \pre `size() < capacity()`
  /// \post `size() > 0 && size() <= capacity()`
  constexpr auto push_back(const_reference value) noexcept -> reference {
    assert(size_ < Capacity);
    impl_[size_++] = value;
    return impl_[size_ - 1];
  }

  ///
  /// \tparam Args
  /// \param args
  /// \return
  /// \pre `size() < capacity()`
  /// \post `size() > 0 && size() <= capacity()`
  template <class... Args>
  constexpr auto emplace_back(Args &&... args) noexcept(std::is_trivially_move_assignable_v<T>) -> reference {
    assert(size_ < Capacity);
    impl_[size_++] = value_type{std::forward<Args>(args)...};
    return back();
  }

  ///
  /// \pre `size() > 0`
  constexpr void pop_back() noexcept {
    assert(size_ > 0);
    back().~value_type();
    --size_;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto data() noexcept -> pointer {
    return impl_.data();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto data() const noexcept -> const_pointer {
    return impl_.data();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return size_;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
    return Capacity;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
    return Capacity;
  }

  ///
  /// \return `true` if there are no elements
  [[nodiscard]] constexpr auto empty() const noexcept -> bool {
    return size_ == 0;
  }

  ///
  /// \post size() == 0
  constexpr void clear() noexcept {
    while (size_) {
      pop_back();
    }
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto begin() noexcept -> iterator {
    return impl_.begin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto end() noexcept -> iterator {
    auto last = impl_.begin();
    std::advance(last, size_);
    return last;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
    return impl_.begin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
    auto last = impl_.begin();
    std::advance(last, size_);
    return last;
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
    return cbegin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
    return cend();
  }
};
}  // namespace skyr::inline v2

#endif  // SKYR_V2_CONTAINERS_STATIC_VECTOR_HPP
