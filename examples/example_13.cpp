// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <print>

#include <skyr/url.hpp>
#include <skyr/url_format.hpp>

int main() {
  auto url = skyr::url("https://user:pass@api.example.com:8080/v1/users?filter=active&limit=10#results");

  std::println("URL Formatting Examples:");
  std::println("========================\n");

  // Full URL (default)
  std::println("Full URL:     {}", url);
  std::println();

  // Individual components (encoded by default)
  std::println("Scheme:       {:s}", url);
  std::println("Hostname:     {:h}", url);
  std::println("Port:         {:p}", url);
  std::println("Pathname:     {:P}", url);
  std::println("Query:        {:q}", url);
  std::println("Fragment:     {:f}", url);
  std::println("Origin:       {:o}", url);
  std::println();

  // Practical examples
  std::println("API Endpoint: {:o}{:P}", url, url);
  std::println("Host:Port:    {:h}:{:p}", url, url);
  std::println();

  // Different URL types
  auto https_url = skyr::url("https://example.com/secure");
  auto http_url = skyr::url("http://example.org:3000/api");
  auto file_url = skyr::url("file:///Users/test/document.txt");

  std::println("HTTPS (default port): {:o}", https_url);
  std::println("HTTP (custom port):   {:o}", http_url);
  std::println("File path:            {:P}", file_url);
  std::println();

  // Logging examples
  std::println("Logging Examples:");
  std::println("-----------------");
  std::println("[INFO] Connecting to {:h}:{:p}", url, url);
  std::println("[DEBUG] Request path: {:P}{:q}", url, url);
  std::println("[TRACE] Full endpoint: {:o}{:P}", url, url);
  std::println();

  // IP addresses
  auto ipv4_url = skyr::url("http://192.168.1.1:8080/admin");
  auto ipv6_url = skyr::url("http://[2001:db8::1]:8080/api");

  std::println("IPv4: {:h}:{:p}", ipv4_url, ipv4_url);
  std::println("IPv6: {:h}:{:p}", ipv6_url, ipv6_url);
  std::println();

  // Decoded output examples
  auto encoded_url = skyr::url("http://example.إختبار/hello%20world?name=John%20Doe&city=Paisley#section%201");

  std::println("Encoded vs Decoded:");
  std::println("-------------------");
  std::println("Hostname (encoded): {:h}", encoded_url);
  std::println("Hostname (decoded): {:hd}", encoded_url);
  std::println();
  std::println("Pathname (encoded): {:P}", encoded_url);
  std::println("Pathname (decoded): {:Pd}", encoded_url);
  std::println();
  std::println("Query (encoded):    {:q}", encoded_url);
  std::println("Query (decoded):    {:qd}", encoded_url);
  std::println();
  std::println("Fragment (encoded): {:f}", encoded_url);
  std::println("Fragment (decoded): {:fd}", encoded_url);
  std::println();

  // Human-readable logging
  std::println("Human-Readable Logging:");
  std::println("-----------------------");
  std::println("[INFO] User requested: {:Pd}", encoded_url);
  std::println("[DEBUG] Query params: {:qd}", encoded_url);
  std::println("[TRACE] Host: {:hd}, Path: {:Pd}", encoded_url, encoded_url);

  return 0;
}
