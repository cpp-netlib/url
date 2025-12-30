// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// WPT (Web Platform Tests) Conformance Tests
// These tests are based on failures from the official WPT URL test suite.
// They focus on high-value conformance issues worth fixing.

#include <exception>

#include <catch2/catch_all.hpp>

#include <skyr/url.hpp>

TEST_CASE("wpt_whitespace_handling", "[url][wpt][whitespace]") {
  using namespace std::string_literals;

  SECTION("tab_in_hostname_should_be_stripped") {
    // WPT: "foo://ho\tst/" should parse as "foo://host/"
    auto url = skyr::make_url("foo://ho\tst/");
    REQUIRE(url);
    CHECK(url->hostname() == "host");
    CHECK(url->host() == "host");
    CHECK(url->href() == "foo://host/");
  }

  SECTION("newline_in_hostname_should_be_stripped") {
    // WPT: "foo://ho\nst/" should parse as "foo://host/"
    auto url = skyr::make_url("foo://ho\nst/");
    REQUIRE(url);
    CHECK(url->hostname() == "host");
    CHECK(url->host() == "host");
    CHECK(url->href() == "foo://host/");
  }

  SECTION("carriage_return_in_hostname_should_be_stripped") {
    // WPT: "foo://host/" (with embedded \r) should parse as "foo://host/"
    auto url = skyr::make_url("foo://ho\rst/");
    REQUIRE(url);
    CHECK(url->hostname() == "host");
    CHECK(url->host() == "host");
    CHECK(url->href() == "foo://host/");
  }

  SECTION("http_tab_in_hostname_should_be_stripped") {
    // WPT: "http://ho\tst/" should parse as "http://host/"
    auto url = skyr::make_url("http://ho\tst/");
    REQUIRE(url);
    CHECK(url->hostname() == "host");
    CHECK(url->host() == "host");
    CHECK(url->href() == "http://host/");
  }

  SECTION("http_newline_in_hostname_should_be_stripped") {
    // WPT: "http://ho\nst/" should parse as "http://host/"
    auto url = skyr::make_url("http://ho\nst/");
    REQUIRE(url);
    CHECK(url->hostname() == "host");
    CHECK(url->host() == "host");
    CHECK(url->href() == "http://host/");
  }

  SECTION("http_carriage_return_in_hostname_should_be_stripped") {
    // WPT: "http://host/" (with embedded \r) should parse as "http://host/"
    auto url = skyr::make_url("http://ho\rst/");
    REQUIRE(url);
    CHECK(url->hostname() == "host");
    CHECK(url->host() == "host");
    CHECK(url->href() == "http://host/");
  }

  SECTION("tab_and_newline_in_hostname_base_url") {
    // WPT: "http://example\t.\norg" with base "http://example.org/foo/bar"
    // Should parse as "http://example.org/"
    auto base = skyr::url("http://example.org/foo/bar");
    auto url = skyr::make_url("http://example\t.\norg", base);
    REQUIRE(url);
    CHECK(url->hostname() == "example.org");
    CHECK(url->host() == "example.org");
    CHECK(url->href() == "http://example.org/");
  }

  SECTION("newline_in_authority_should_parse") {
    // WPT: "http://f:\n/c" with base should parse successfully
    auto base = skyr::url("http://example.org/foo/bar");
    auto url = skyr::make_url("http://f:\n/c", base);
    CHECK(url);  // Should not fail to parse
  }

  SECTION("tabs_in_non_special_opaque_path") {
    // WPT: "non-special:opaque \t\t  \t#hi" should collapse tabs/spaces
    // Expected pathname: "opaque  %20" (tabs converted to spaces, then one encoded)
    auto url = skyr::make_url("non-special:opaque \t\t  \t#hi");
    REQUIRE(url);
    CHECK(url->pathname() == "opaque  %20");
    CHECK(url->href() == "non-special:opaque  %20#hi");
  }

  SECTION("tabs_in_entire_url") {
    // WPT: Complex case with tabs throughout
    // "h\tt\ntp://h\to\nst:9\t0\n00/p\ta\nth?q\tu\nery#f\tr\nag"
    // Should parse as "http://host:9000/path?query#frag"
    auto url = skyr::make_url("h\tt\ntp://h\to\nst:9\t0\n00/p\ta\nth?q\tu\nery#f\tr\nag");
    REQUIRE(url);
    CHECK(url->protocol() == "http:");
    CHECK(url->hostname() == "host");
    CHECK(url->host() == "host:9000");
    CHECK(url->port() == "9000");
    CHECK(url->pathname() == "/path");
    CHECK(url->search() == "?query");
    CHECK(url->hash() == "#frag");
    CHECK(url->href() == "http://host:9000/path?query#frag");
  }

  SECTION("newline_in_file_url_path") {
    // WPT: "C|\n/" with base "file://host/dir/file"
    // Expected: "file://host/C:/" with pathname "/C:/"
    auto base = skyr::url("file://host/dir/file");
    auto url = skyr::make_url("C|\n/", base);
    REQUIRE(url);
    CHECK(url->pathname() == "/C:/");
    CHECK(url->href() == "file://host/C:/");
  }
}

TEST_CASE("wpt_percent_encoding", "[url][wpt][percent-encoding]") {
  using namespace std::string_literals;

  SECTION("spaces_in_non_special_path_before_query") {
    // WPT: "non-special:opaque  ?hi"
    // Expected pathname: "opaque %20" (trailing spaces should be percent-encoded)
    auto url = skyr::make_url("non-special:opaque  ?hi");
    REQUIRE(url);
    CHECK(url->pathname() == "opaque %20");
    CHECK(url->href() == "non-special:opaque %20?hi");
  }

  SECTION("spaces_in_non_special_path_before_fragment") {
    // WPT: "non-special:opaque  #hi"
    // Expected pathname: "opaque %20" (trailing spaces should be percent-encoded)
    auto url = skyr::make_url("non-special:opaque  #hi");
    REQUIRE(url);
    CHECK(url->pathname() == "opaque %20");
    CHECK(url->href() == "non-special:opaque %20#hi");
  }

  SECTION("password_with_special_chars_at_sign_colon") {
    // WPT: "http://::@c@d:2" with base "http://example.org/foo/bar"
    // Username should be empty, password should be "%3A%40c"
    auto base = skyr::url("http://example.org/foo/bar");
    auto url = skyr::make_url("http://::@c@d:2", base);
    REQUIRE(url);
    CHECK(url->username() == "");
    CHECK(url->password() == "%3A%40c");
    CHECK(url->hostname() == "d");
    CHECK(url->port() == "2");
    CHECK(url->href() == "http://:%3A%40c@d:2/");
  }

  SECTION("password_encoding_special_characters_non_special_scheme") {
    // WPT: "foo://joe: !\"$%&'()*+,-.:;<=>@[\\]^_`{|}~@host/"
    // Username: "joe", Password should encode special chars including @ within password
    auto url = skyr::make_url("foo://joe: !\"$%&'()*+,-.:;<=>@[\\]^_`{|}~@host/");
    REQUIRE(url);
    CHECK(url->username() == "joe");
    CHECK(url->password() == "%20!%22$%&'()*+,-.%3A%3B%3C%3D%3E%40%5B%5C%5D%5E_%60%7B%7C%7D~");
    CHECK(url->hostname() == "host");
    CHECK(url->href() == "foo://joe:%20!%22$%&'()*+,-.%3A%3B%3C%3D%3E%40%5B%5C%5D%5E_%60%7B%7C%7D~@host/");
  }

  SECTION("password_encoding_special_characters_wss_scheme") {
    // WPT: "wss://joe: !\"$%&'()*+,-.:;<=>@[]^_`{|}~@host/"
    // Similar to above but for wss:// (special scheme)
    auto url = skyr::make_url("wss://joe: !\"$%&'()*+,-.:;<=>@[]^_`{|}~@host/");
    REQUIRE(url);
    CHECK(url->username() == "joe");
    CHECK(url->password() == "%20!%22$%&'()*+,-.%3A%3B%3C%3D%3E%40%5B%5D%5E_%60%7B%7C%7D~");
    CHECK(url->hostname() == "host");
    CHECK(url->href() == "wss://joe:%20!%22$%&'()*+,-.%3A%3B%3C%3D%3E%40%5B%5D%5E_%60%7B%7C%7D~@host/");
  }

  SECTION("caret_encoding_in_non_special_path") {
    // WPT: "foo://host/ !\"$%&'()*+,-./:;<=>@[\\]^_`{|}~"
    // Caret (^) should be encoded as %5E in path
    auto url = skyr::make_url("foo://host/ !\"$%&'()*+,-./:;<=>@[\\]^_`{|}~");
    REQUIRE(url);
    CHECK(url->pathname() == "/%20!%22$%&'()*+,-./:;%3C=%3E@[\\]%5E_%60%7B|%7D~");
    CHECK(url->href() == "foo://host/%20!%22$%&'()*+,-./:;%3C=%3E@[\\]%5E_%60%7B|%7D~");
  }

  SECTION("caret_encoding_in_wss_path") {
    // WPT: "wss://host/ !\"$%&'()*+,-./:;<=>@[\\]^_`{|}~"
    // Caret (^) should be encoded as %5E in path
    auto url = skyr::make_url("wss://host/ !\"$%&'()*+,-./:;<=>@[\\]^_`{|}~");
    REQUIRE(url);
    CHECK(url->pathname() == "/%20!%22$%&'()*+,-./:;%3C=%3E@[/]%5E_%60%7B|%7D~");
    CHECK(url->href() == "wss://host/%20!%22$%&'()*+,-./:;%3C=%3E@[/]%5E_%60%7B|%7D~");
  }
}

TEST_CASE("wpt_invalid_characters", "[url][wpt][validation]") {
  using namespace std::string_literals;

  SECTION("pipe_in_non_special_hostname_should_fail") {
    // WPT: "sc://a|b/" should fail to parse (pipe character invalid in host)
    auto url = skyr::make_url("sc://a|b/");
    CHECK_FALSE(url);
  }

  // Note: Many of the "Invalid Character Accepted" failures involve
  // invisible/control characters that are hard to represent in source code.
  // These would need to be constructed programmatically or read from test data files.
  // The WPT runner already tests these comprehensively.
}