// Copyright 2020 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_ALLOCATIONS_HPP
#define SKYR_ALLOCATIONS_HPP

#include <iostream>
#include <cstdlib>
#include <optional>
#include <fmt/format.h>


auto num_allocations = std::optional<unsigned int>{};

struct skyr_allocation_test_runner {
  skyr_allocation_test_runner() {
    num_allocations = 0u;
  }
  ~skyr_allocation_test_runner() {
    std::cout << "There were " << num_allocations.value() << " allocations\n\n";
  }
};

auto operator new (std::size_t num_bytes) -> void * {
  if (num_allocations) {
    std::cout << "[" << fmt::format("{:3d}", ++num_allocations.value()) << "] " << num_bytes << std::endl;
  }
  return std::malloc(num_bytes); // NOLINT
}

void operator delete (void *p) noexcept {
  std::free(p); // NOLINT
}


#define SKYR_ALLOCATIONS_START_COUNTING(message) \
  std::cout << "::::: " << message << " ----- \n"; \
  [[maybe_unused]] auto counting_object_ = skyr_allocation_test_runner();


#endif  // SKYR_ALLOCATIONS_HPP
