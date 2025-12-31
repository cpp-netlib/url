// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <format>
#include <iostream>
#include <string_view>
#include <vector>

#include <skyr/url.hpp>

// Comprehensive URL sanitizer tests covering edge cases and diverse inputs
// This test is designed to stress-test the URL parser with AddressSanitizer
// to detect any memory safety issues (buffer overflows, use-after-free, etc.)

struct test_case {
  std::string input;
  std::string description;
};

int main() {
  // Test cases covering a wide range of URL patterns to stress-test with ASan
  const std::vector<test_case> test_cases = {
      // Basic URLs
      {"http://example.com", "Simple HTTP URL"},
      {"https://example.com", "Simple HTTPS URL"},
      {"ftp://ftp.example.com", "FTP URL"},
      {"file:///path/to/file", "File URL"},

      // URLs with ports
      {"http://example.com:8080", "HTTP with port"},
      {"https://example.com:443", "HTTPS with default port"},
      {"http://example.com:0", "Port 0 (edge case)"},
      {"http://example.com:65535", "Maximum valid port"},
      {"http://example.com:65536", "Port overflow"},
      {"http://example.com:99999", "Port out of range"},

      // URLs with authentication
      {"http://user:pass@example.com", "URL with credentials"},
      {"http://user@example.com", "URL with username only"},
      {"http://:pass@example.com", "URL with password only"},
      {"http://user%20name:pass%20word@example.com", "Encoded credentials"},

      // IPv4 addresses
      {"http://192.168.1.1", "IPv4 address"},
      {"http://127.0.0.1:8080", "Localhost with port"},
      {"http://255.255.255.255", "Max IPv4 address"},
      {"http://256.1.1.1", "Invalid IPv4 (overflow)"},
      {"http://0.0.0.0", "Zero IPv4 address"},

      // IPv6 addresses
      {"http://[::1]", "IPv6 loopback"},
      {"http://[2001:db8::1]", "IPv6 address"},
      {"http://[::ffff:192.0.2.1]", "IPv4-mapped IPv6"},
      {"http://[2001:db8::1]:8080", "IPv6 with port"},
      {"http://[::1]:65536", "IPv6 with invalid port"},

      // Path components
      {"http://example.com/path/to/resource", "URL with path"},
      {"http://example.com/path/../other", "URL with dot segments"},
      {"http://example.com/./path", "URL with single dot"},
      {"http://example.com/../path", "URL starting with .."},
      {"http://example.com//double//slash", "Double slashes in path"},

      // Query strings
      {"http://example.com?key=value", "URL with query"},
      {"http://example.com?key1=value1&key2=value2", "Multiple query params"},
      {"http://example.com?key=", "Empty query value"},
      {"http://example.com?=value", "Empty query key"},
      {"http://example.com?", "Empty query string"},
      {"http://example.com?key=value%20with%20spaces", "Encoded query"},

      // Fragments
      {"http://example.com#fragment", "URL with fragment"},
      {"http://example.com#", "Empty fragment"},
      {"http://example.com#fragment%20with%20spaces", "Encoded fragment"},
      {"http://example.com?query=1#fragment", "Query and fragment"},

      // Percent encoding edge cases (potential for buffer issues)
      {"http://example.com/%20", "Encoded space"},
      {"http://example.com/%00", "Null byte encoded"},
      {"http://example.com/%", "Incomplete encoding"},
      {"http://example.com/%2", "Incomplete encoding 2"},
      {"http://example.com/%GG", "Invalid hex encoding"},
      {"http://example.com/%C3%A9", "UTF-8 encoded character"},

      // Unicode and internationalized domains (potential encoding issues)
      {"http://\xE2\x98\x83.example.com", "Snowman in domain"},
      {"http://\xF0\x9F\x92\xA9.example.com", "Emoji in domain"},
      {"http://münchen.de", "German umlaut domain"},
      {"http://\xE4\xB8\xAD\xE5\x9B\xBD.cn", "Chinese domain"},
      {"http://example.com/\xF0\x9F\x92\xA9", "Emoji in path"},

      // Special schemes
      {"data:text/plain,Hello", "Data URL"},
      {"mailto:user@example.com", "Mailto URL"},
      {"tel:+1-234-567-8900", "Tel URL"},
      {"javascript:alert('xss')", "JavaScript URL"},
      {"about:blank", "About URL"},

      // Edge cases and malformed URLs (boundary conditions)
      {"http://", "No host"},
      {"http:///path", "Empty host"},
      {"//example.com", "Protocol-relative URL"},
      {"/path/to/resource", "Path-only URL"},
      {"http://example.com:abc", "Non-numeric port"},
      {"http://exam ple.com", "Space in host"},
      {"http://example..com", "Double dot in domain"},
      {"http://.example.com", "Leading dot in domain"},
      {"http://example.com.", "Trailing dot in domain"},

      // Very long URLs (stress test buffers)
      {"http://example.com/" + std::string(1000, 'a'), "Very long path (1KB)"},
      {"http://example.com/" + std::string(10000, 'x'), "Very long path (10KB)"},
      {"http://" + std::string(253, 'a') + ".com", "Very long domain (253 chars)"},
      {"http://example.com?" + std::string(1000, 'q'), "Very long query (1KB)"},
      {"http://example.com#" + std::string(1000, 'f'), "Very long fragment (1KB)"},

      // Special characters (potential for injection or buffer issues)
      {"http://example.com/path?key=<script>", "HTML in query"},
      {"http://example.com/path?key=\xF0\x9F\x92\xA9", "Emoji in query"},
      {"http://example.com/\x00\x01\x02", "Control characters"},
      {"http://example.com/\xFF\xFE", "Invalid UTF-8"},

      // Backslash handling (Windows path edge cases)
      {"http://example.com\\path", "Backslash in path"},
      {"http:\\\\example.com", "Backslashes instead of slashes"},

      // Empty and whitespace (boundary conditions)
      {"", "Empty string"},
      {" ", "Single space"},
      {"\t", "Tab character"},
      {"\n", "Newline character"},
      {"  http://example.com  ", "URL with surrounding whitespace"},

      // Case sensitivity
      {"HTTP://EXAMPLE.COM", "Uppercase scheme and host"},
      {"hTtP://eXaMpLe.CoM", "Mixed case"},

      // Punycode (IDNA encoding)
      {"http://xn--nxasmq6b.example.com", "Punycode domain"},
      {"http://xn--ls8h.example.com", "Punycode emoji"},

      // Multiple encoding/decoding rounds (potential for bugs)
      {"http://example.com/%252F", "Double-encoded slash"},
      {"http://example.com/%25%32%46", "Triple-encoded slash"},

      // Null and boundary values
      {"http://example.com/" + std::string(1, '\0') + "path", "Embedded null byte"},
      {"http://example.com/\x7F", "DEL character"},

      // Repeated characters (stress test for buffer operations)
      {"http://example.com/" + std::string(100, '/'), "Many slashes"},
      {"http://example.com?" + std::string(100, '&'), "Many ampersands"},
      {"http://example.com#" + std::string(100, '#'), "Many hashes"},
  };

  std::cout << std::format("Running {} URL test cases with AddressSanitizer + UndefinedBehaviorSanitizer\n",
                           test_cases.size());
  std::cout << "The goal is to detect memory safety issues, not validate parsing correctness.\n";
  std::cout << "If sanitizers detect issues (buffer overflow, use-after-free, etc.), the program will abort.\n\n";

  size_t test_number = 0;
  for (const auto& tc : test_cases) {
    test_number++;

    try {
      auto url_result = skyr::url(tc.input);
      // URL parsed successfully - sanitizers checked for memory issues
      std::cout << std::format("[{:3}] {} - parsed\n", test_number, tc.description);
    } catch (const std::exception&) {
      // URL parsing failed (rejected as invalid) - sanitizers still checked for memory issues
      std::cout << std::format("[{:3}] {} - rejected\n", test_number, tc.description);
    }
  }

  std::cout << std::format("\n{}\n", std::string(80, '='));
  std::cout << std::format("✓ All {} tests completed successfully!\n", test_number);
  std::cout << "No memory safety issues detected by AddressSanitizer or UndefinedBehaviorSanitizer.\n";
  std::cout << std::format("{}\n", std::string(80, '='));

  return 0;
}