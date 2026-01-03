// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <skyr/url.hpp>

int main() {
  // Immutable URL transformations - original URL never changes
  auto url = skyr::url("http://localhost:3000/api/v1/users");

  std::cout << "Original URL:\n";
  std::cout << "  " << url.href() << "\n\n";

  // Individual transformations return new URLs
  if (auto result = url.with_scheme("https")) {
    std::cout << "Change scheme:\n";
    std::cout << "  " << result->href() << "\n\n";
  }

  if (auto result = url.with_hostname("api.example.com")) {
    std::cout << "Change hostname:\n";
    std::cout << "  " << result->href() << "\n\n";
  }

  // Original URL is unchanged
  std::cout << "Original still unchanged:\n";
  std::cout << "  " << url.href() << "\n\n";

  // Chain transformations with .and_then()
  auto prod_url = url.with_scheme("https")
                      .and_then([](auto&& u) { return u.with_hostname("api.example.com"); })
                      .and_then([](auto&& u) { return u.with_port(""); })
                      .and_then([](auto&& u) { return u.with_pathname("/api/v2/users"); })
                      .and_then([](auto&& u) { return u.with_search("limit=100"); });

  if (prod_url) {
    std::cout << "Chained transformations (dev → prod):\n";
    std::cout << "  " << prod_url->href() << "\n\n";
  }

  // Build API endpoint
  auto api_base = skyr::url("https://api.example.com");
  auto endpoint =
      api_base.with_pathname("/v1/users").and_then([](auto&& u) { return u.with_search("page=2&limit=50"); });

  if (endpoint) {
    std::cout << "Build API endpoint:\n";
    std::cout << "  " << endpoint->href() << "\n";
  }

  return 0;
}
