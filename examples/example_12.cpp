// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <skyr/url.hpp>

int main() {
  // URL sanitization example
  auto url = skyr::url("https://user:pass@example.com/path?foo=1&bar=2&baz=3#fragment");

  std::cout << "Original URL:\n";
  std::cout << "  " << url.href() << "\n\n";

  // Sanitize: remove credentials and fragment
  auto sanitized_url = url.sanitize();
  std::cout << "Sanitized (credentials & fragment removed):\n";
  std::cout << "  " << sanitized_url.href() << "\n\n";

  // Remove query string
  auto sanitized_url_without_query = url.without_query();
  std::cout << "Without query:\n";
  std::cout << "  " << sanitized_url_without_query.href() << "\n\n";

  // Remove fragment
  auto sanitized_url_without_fragment = url.without_fragment();
  std::cout << "Without fragment:\n";
  std::cout << "  " << sanitized_url_without_fragment.href() << "\n\n";

  // Remove specific query parameters
  auto sanitized_url_filtered = url.without_params({"bar"});
  std::cout << "Remove 'bar' parameter:\n";
  std::cout << "  " << sanitized_url_filtered.href() << "\n\n";

  // Chain operations for fully clean URL
  auto sanitized_url_fully_clean = url.sanitize().without_query();
  std::cout << "Fully clean (sanitize + remove query):\n";
  std::cout << "  " << sanitized_url_fully_clean.href() << "\n\n";

  // Complex chaining
  auto sanitized_url_clean_filtered = url.sanitize().without_params({"bar", "baz"});
  std::cout << "Sanitize + remove 'bar' and 'baz':\n";
  std::cout << "  " << sanitized_url_clean_filtered.href() << "\n";

  return 0;
}
