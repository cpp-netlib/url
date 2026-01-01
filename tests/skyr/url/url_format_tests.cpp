// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <exception>
#include <format>

#include <catch2/catch_all.hpp>

#include <skyr/url.hpp>
#include <skyr/url_format.hpp>

TEST_CASE("url_format_tests", "[url][format]") {
  using namespace std::string_literals;

  auto url = skyr::url("https://user:pass@example.com:8080/path?query=1#fragment");

  SECTION("format_full_url_default") {
    auto result = std::format("{}", url);
    CHECK(result == "https://user:pass@example.com:8080/path?query=1#fragment");
  }

  SECTION("format_full_url_explicit") {
    auto result = std::format("{:}", url);
    CHECK(result == "https://user:pass@example.com:8080/path?query=1#fragment");
  }

  SECTION("format_scheme") {
    auto result = std::format("{:s}", url);
    CHECK(result == "https");
  }

  SECTION("format_hostname") {
    auto result = std::format("{:h}", url);
    CHECK(result == "example.com");
  }

  SECTION("format_port") {
    auto result = std::format("{:p}", url);
    CHECK(result == "8080");
  }

  SECTION("format_pathname") {
    auto result = std::format("{:P}", url);
    CHECK(result == "/path");
  }

  SECTION("format_query") {
    auto result = std::format("{:q}", url);
    CHECK(result == "?query=1");
  }

  SECTION("format_fragment") {
    auto result = std::format("{:f}", url);
    CHECK(result == "#fragment");
  }

  SECTION("format_origin") {
    auto result = std::format("{:o}", url);
    CHECK(result == "https://example.com:8080");
  }

  SECTION("format_default_port_omitted") {
    auto url_default = skyr::url("https://example.com/path");
    auto result = std::format("{:p}", url_default);
    CHECK(result.empty());
  }

  SECTION("format_no_query") {
    auto url_no_query = skyr::url("https://example.com/path");
    auto result = std::format("{:q}", url_no_query);
    CHECK(result.empty());
  }

  SECTION("format_no_fragment") {
    auto url_no_fragment = skyr::url("https://example.com/path");
    auto result = std::format("{:f}", url_no_fragment);
    CHECK(result.empty());
  }

  SECTION("format_multiple_specs_in_string") {
    auto result = std::format("Hostname: {:h}, Port: {:p}, Path: {:P}", url, url, url);
    CHECK(result == "Hostname: example.com, Port: 8080, Path: /path");
  }

  SECTION("format_combined_with_text") {
    auto result = std::format("Visit {} for more info", url);
    CHECK(result == "Visit https://user:pass@example.com:8080/path?query=1#fragment for more info");
  }

  SECTION("format_origin_for_http") {
    auto http_url = skyr::url("http://example.org:3000/");
    auto result = std::format("{:o}", http_url);
    CHECK(result == "http://example.org:3000");
  }

  SECTION("format_origin_with_default_port") {
    auto https_url = skyr::url("https://example.com/path");
    auto result = std::format("{:o}", https_url);
    CHECK(result == "https://example.com");
  }

  SECTION("format_ipv4_hostname") {
    auto ipv4_url = skyr::url("http://192.168.1.1:8080/");
    auto result = std::format("{:h}", ipv4_url);
    CHECK(result == "192.168.1.1");
  }

  SECTION("format_ipv6_hostname") {
    auto ipv6_url = skyr::url("http://[2001:db8::1]/");
    auto result = std::format("{:h}", ipv6_url);
    CHECK(result == "[2001:db8::1]");
  }

  SECTION("format_international_domain") {
    auto idn_url = skyr::url("http://example.xn--kgbechtv/");
    auto result = std::format("{:h}", idn_url);
    CHECK(result == "example.xn--kgbechtv");
  }

  SECTION("format_file_url") {
    auto file_url = skyr::url("file:///Users/test/file.txt");
    auto result = std::format("{:P}", file_url);
    CHECK(result == "/Users/test/file.txt");
  }

  // Decoded format specs
  SECTION("format_hostname_decoded") {
    auto idn_url = skyr::url("http://example.xn--kgbechtv/");
    auto encoded = std::format("{:h}", idn_url);
    auto decoded = std::format("{:hd}", idn_url);

    CHECK(encoded == "example.xn--kgbechtv");
    CHECK(decoded == "example.إختبار");
  }

  SECTION("format_pathname_decoded") {
    auto url_encoded = skyr::url("http://example.com/hello%20world");
    auto encoded = std::format("{:P}", url_encoded);
    auto decoded = std::format("{:Pd}", url_encoded);

    CHECK(encoded == "/hello%20world");
    CHECK(decoded == "/hello world");
  }

  SECTION("format_pathname_with_unicode_decoded") {
    auto url_unicode = skyr::url("http://example.com/%CF%80");  // π
    auto encoded = std::format("{:P}", url_unicode);
    auto decoded = std::format("{:Pd}", url_unicode);

    CHECK(encoded == "/%CF%80");
    CHECK(decoded == "/π");
  }

  SECTION("format_query_decoded") {
    auto url_query = skyr::url("http://example.com/?name=John%20Doe&city=Paisley");
    auto encoded = std::format("{:q}", url_query);
    auto decoded = std::format("{:qd}", url_query);

    CHECK(encoded == "?name=John%20Doe&city=Paisley");
    CHECK(decoded == "?name=John Doe&city=Paisley");
  }

  SECTION("format_fragment_decoded") {
    auto url_frag = skyr::url("http://example.com/#section%201");
    auto encoded = std::format("{:f}", url_frag);
    auto decoded = std::format("{:fd}", url_frag);

    CHECK(encoded == "#section%201");
    CHECK(decoded == "#section 1");
  }

  SECTION("format_decoded_combined") {
    auto url_full = skyr::url("http://example.إختبار/path%20name?q=hello%20world#top%20section");
    auto result =
        std::format("Host: {:hd}, Path: {:Pd}, Query: {:qd}, Fragment: {:fd}", url_full, url_full, url_full, url_full);

    CHECK(result == "Host: example.إختبار, Path: /path name, Query: ?q=hello world, Fragment: #top section");
  }

  SECTION("format_decoded_fallback_on_invalid") {
    // If decoding fails, should fall back to encoded version
    auto url_invalid = skyr::url("http://example.com/%FF%FE");  // Invalid UTF-8
    auto decoded = std::format("{:Pd}", url_invalid);

    // Should fall back to encoded version
    CHECK(decoded == "/%FF%FE");
  }

  SECTION("format_hostname_ascii_decoded") {
    // ASCII hostname should work fine with or without decode
    auto url_ascii = skyr::url("http://example.com/");
    auto encoded = std::format("{:h}", url_ascii);
    auto decoded = std::format("{:hd}", url_ascii);

    CHECK(encoded == "example.com");
    CHECK(decoded == "example.com");
  }

  // Note: Invalid format specs (like {:x} or {:sh}) are caught at compile-time,
  // not runtime, so we can't test them here. The compiler will reject invalid specs.
}
