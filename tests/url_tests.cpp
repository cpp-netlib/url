// Copyright 2010 Jeroen Habraken.
// Copyright 2009-2019 Dean Michael Berris == Glyn Matthews.
// Copyright 2012 Google == Inc.
// Distributed under the Boost Software License == Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <memory>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/url.hpp>

TEST_CASE("url_tests", "[url]") {
#if __cpp_exceptions
  SECTION("construct_invalid_url") {
    REQUIRE_THROWS_AS(skyr::url("I am not a valid url."), skyr::url_parse_error);
  }
#endif // __cpp_exceptions

  SECTION("construct_invalid_url_make") {
    CHECK_FALSE(skyr::make_url("I am not a valid url."));
  }

#if __cpp_exceptions
  SECTION("construct_url_from_char_array") {
    REQUIRE_NOTHROW(skyr::url("http://www.example.com/"));
  }
#endif // __cpp_exceptions

  SECTION("construct_url_from_char_array_make") {
    CHECK(skyr::make_url("http://www.example.com/"));
  }

#if __cpp_exceptions
  SECTION("construct_url_starting_with_ipv4_like)") {
    REQUIRE_NOTHROW(skyr::url("http://198.51.100.0.example.com/"));
  }
#endif // __cpp_exceptions

  SECTION("construct_url_starting_with_ipv4_like_make") {
    CHECK(skyr::make_url("http://198.51.100.0.example.com/"));
  }

#if __cpp_exceptions
  SECTION("construct_url_starting_with_ipv4_like_glued") {
    REQUIRE_NOTHROW(skyr::url("http://198.51.100.0example.com/"));
  }
#endif // __cpp_exceptions

  SECTION("construct_url_starting_with_ipv4_like_glued_make") {
    REQUIRE(skyr::make_url("http://198.51.100.0example.com/"));
  }

#if __cpp_exceptions
  SECTION("construct_url_like_short_ipv4") {
    REQUIRE_NOTHROW(skyr::url("http://198.51.100/"));
  }
#endif // __cpp_exceptions

  SECTION("construct_url_like_short_ipv4_make") {
    CHECK(skyr::make_url("http://198.51.100/"));
  }

#if __cpp_exceptions
  SECTION("construct_url_like_long_ipv4") {
    REQUIRE_NOTHROW(skyr::url("http://198.51.100.0.255/"));
  }
#endif // __cpp_exceptions

  SECTION("construct_url_like_long_ipv4_make)") {
    CHECK(skyr::make_url("http://198.51.100.0.255/"));
  }

#if __cpp_exceptions
  SECTION("construct_url_from_string") {
    auto input = std::string("http://www.example.com/");
    REQUIRE_NOTHROW((skyr::url(input)));
  }
#endif // __cpp_exceptions

  SECTION("construct_url_from_string_make") {
    auto input = std::string("http://www.example.com/");
    CHECK(skyr::make_url(input));
  }

#if __cpp_exceptions
  SECTION("basic_url_protocol_test") {
    auto instance = skyr::url("http://www.example.com/");
    CHECK("http:" == instance.protocol());
  }

  SECTION("basic_url_user_info_test") {
    auto instance = skyr::url("http://www.example.com/");
    CHECK(instance.username().empty());
    CHECK(instance.password().empty());
  }

  SECTION("basic_url_host_test") {
    auto instance = skyr::url("http://www.example.com/");
    CHECK("www.example.com" == instance.host());
  }

  SECTION("basic_url_port_test") {
    auto instance = skyr::url("http://www.example.com/");
    CHECK(instance.port().empty());
  }

  SECTION("basic_url_path_test") {
    auto instance = skyr::url("http://www.example.com/");
    CHECK("/" == instance.pathname());
  }

  SECTION("basic_url_search_test") {
    auto instance = skyr::url("http://www.example.com/");
    CHECK(instance.search().empty());
  }

  SECTION("basic_url_hash_test") {
    auto instance = skyr::url("http://www.example.com/");
    CHECK(instance.hash().empty());
  }

  SECTION("full_url_scheme_test") {
    auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
    CHECK("http:" == instance.protocol());
  }

  SECTION("full_url_user_info_test") {
    auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
    CHECK("user" == instance.username());
    CHECK(instance.password().empty());
  }

  SECTION("full_url_host_with_default_port_test") {
    auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
    CHECK("www.example.com" == instance.host());
  }

  SECTION("full_url_hostname_with_default_port_test") {
    auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
    CHECK("www.example.com" == instance.hostname());
  }

  SECTION("full_url_port_with_default_port_test") {
    auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
    CHECK(instance.port().empty());
  }

  SECTION("full_url_host_with_non_default_port_test") {
    auto instance = skyr::url("http://user@www.example.com:8080/path?query#fragment");
    CHECK("www.example.com:8080" == instance.host());
  }

  SECTION("full_url_hostname_with_non_default_port_test") {
    auto instance = skyr::url("http://user@www.example.com:8080/path?query#fragment");
    CHECK("www.example.com" == instance.hostname());
  }

  SECTION("full_url_port_with_non_default_port_test") {
    auto instance = skyr::url("http://user@www.example.com:8080/path?query#fragment");
    CHECK("8080" == instance.port());
  }

  SECTION("full_url_port_as_int_test") {
    auto instance = skyr::url("http://www.example.com:8080/");
    CHECK(8080 == instance.port<int>());
  }

  SECTION("full_url_path_test") {
    auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
    CHECK("/path" == instance.pathname());
  }

  SECTION("full_url_search_test") {
    auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
    CHECK("?query" == instance.search());
  }

  SECTION("full_url_hash_test") {
    auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
    CHECK("#fragment" == instance.hash());
  }

  SECTION("url_with_empty_search") {
    auto instance = skyr::url("http://example.com/?");
    CHECK("" == instance.search());
  }

  SECTION("mailto_test") {
    auto instance = skyr::url("mailto:john.doe@example.com");
    CHECK("mailto:" == instance.protocol());
    CHECK("john.doe@example.com" == instance.pathname());
  }

  SECTION("file_test") {
    auto instance = skyr::url("file:///bin/bash");
    CHECK("file:" == instance.protocol());
    CHECK("/bin/bash" == instance.pathname());
  }

  SECTION("xmpp_test") {
    auto instance = skyr::url("xmpp:example-node@example.com?message;subject=Hello%20World");
    CHECK("xmpp:" == instance.protocol());
    CHECK("example-node@example.com" == instance.pathname());
    CHECK("?message;subject=Hello%20World" == instance.search());
  }

  SECTION("ipv4_address_test") {
    auto instance = skyr::url("http://129.79.245.252/");
    CHECK("http:" == instance.protocol());
    CHECK("129.79.245.252" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv4_loopback_test") {
    auto instance = skyr::url("http://127.0.0.1/");
    CHECK("http:" == instance.protocol());
    CHECK("127.0.0.1" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_1") {
    auto instance = skyr::url("http://[1080:0:0:0:8:800:200C:417A]/");
    CHECK("http:" == instance.protocol());
    CHECK("[1080::8:800:200c:417a]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_2") {
    auto instance = skyr::url("http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8:85a3:8d3:1319:8a2e:370:7348]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_3") {
    auto instance = skyr::url("http://[2001:db8:85a3:0:0:8a2e:370:7334]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8:85a3::8a2e:370:7334]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_4") {
    auto instance = skyr::url("http://[2001:db8:85a3::8a2e:370:7334]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8:85a3::8a2e:370:7334]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_5") {
    auto instance = skyr::url("http://[2001:0db8:0000:0000:0000:0000:1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_6") {
    auto instance = skyr::url("http://[2001:0db8:0000:0000:0000::1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_7") {
    auto instance = skyr::url("http://[2001:0db8:0:0:0:0:1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_8") {
    auto instance = skyr::url("http://[2001:0db8:0:0::1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_9") {
    auto instance = skyr::url("http://[2001:0db8::1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_10") {
    auto instance = skyr::url("http://[2001:db8::1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_11") {
    auto instance = skyr::url("http://[::ffff:0c22:384e]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::ffff:c22:384e]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_12") {
    auto instance = skyr::url("http://[fe80::]/");
    CHECK("http:" == instance.protocol());
    CHECK("[fe80::]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_13") {
    auto instance = skyr::url("http://[::ffff:c000:280]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::ffff:c000:280]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_loopback_test") {
    auto instance = skyr::url("http://[::1]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::1]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_loopback_test_1") {
    auto instance = skyr::url("http://[0000:0000:0000:0000:0000:0000:0000:0001]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::1]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_v4inv6_test_1") {
    auto instance = skyr::url("http://[::ffff:12.34.56.78]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::ffff:c22:384e]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_v4inv6_test_2") {
    auto instance = skyr::url("http://[::ffff:192.0.2.128]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::ffff:c000:280]" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("ftp_test") {
    auto instance = skyr::url("ftp://john.doe@ftp.example.com/");
    CHECK("ftp:" == instance.protocol());
    CHECK("john.doe" == instance.username());
    CHECK("ftp.example.com" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("news_test") {
    auto instance = skyr::url("news:comp.infosystems.www.servers.unix");
    CHECK("news:" == instance.protocol());
    CHECK("comp.infosystems.www.servers.unix" == instance.pathname());
  }

  SECTION("tel_test") {
    auto instance = skyr::url("tel:+1-816-555-1212");
    CHECK("tel:" == instance.protocol());
    CHECK("+1-816-555-1212" == instance.pathname());
  }

  SECTION("ldap_test") {
    auto instance = skyr::url("ldap://[2001:db8::7]/c=GB?objectClass?one");
    CHECK("ldap:" == instance.protocol());
    CHECK("[2001:db8::7]" == instance.host());
    CHECK("/c=GB" == instance.pathname());
    CHECK("?objectClass?one" == instance.search());
  }

  SECTION("urn_test") {
    auto instance = skyr::url("urn:oasis:names:specification:docbook:dtd:xml:4.1.2");
    CHECK("urn:" == instance.protocol());
    CHECK("oasis:names:specification:docbook:dtd:xml:4.1.2" == instance.pathname());
  }

  SECTION("svn_ssh_test") {
    auto instance = skyr::url("svn+ssh://example.com/");
    CHECK("svn+ssh:" == instance.protocol());
    CHECK("example.com" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("range_test") {
    const std::string url("http://www.example.com/");
    auto instance = skyr::url(url);
    CHECK(std::equal(std::begin(instance), std::end(instance), begin(url)));
  }

  SECTION("issue_104_test") {
    // https://github.com/cpp-netlib/cpp-netlib/issues/104
    auto instance = std::make_unique<skyr::url>("http://www.example.com/");
    skyr::url copy = *instance;
    instance.reset();
    CHECK("http:" == copy.protocol());
  }

  SECTION("empty_url") {
    auto instance = skyr::url();
    CHECK(instance.empty());
  }

  SECTION("whitespace_no_throw") {
    REQUIRE_NOTHROW(skyr::url(" http://www.example.com/ "));
  }

  SECTION("git") {
    auto instance = skyr::url("git://github.com/cpp-netlib/cpp-netlib.git");
    CHECK("git:" == instance.protocol());
    CHECK("github.com" == instance.host());
    CHECK("/cpp-netlib/cpp-netlib.git" == instance.pathname());
  }

  SECTION("invalid_port_test") {
    REQUIRE_THROWS_AS(skyr::url("http://123.34.23.56:6662626/"), skyr::url_parse_error);
  }

  SECTION("valid_empty_port_test") {
    REQUIRE_NOTHROW(skyr::url("http://123.34.23.56:/"));
  }

  SECTION("empty_port_test") {
    auto instance = skyr::url("http://123.34.23.56:/");
    CHECK("" == instance.port());
  }

  SECTION("nonspecial_url_with_one_slash") {
    REQUIRE_NOTHROW(skyr::url("scheme:/path/"));
  }

  SECTION("url_begins_with_a_colon") {
    REQUIRE_THROWS_AS(skyr::url("://example.com"), skyr::url_parse_error);
  }

  SECTION("url_begins_with_a_number") {
    REQUIRE_THROWS_AS(skyr::url("3http://example.com"), skyr::url_parse_error);
  }

  SECTION("url_scheme_contains_an_invalid_character") {
    REQUIRE_THROWS_AS(skyr::url("ht%tp://example.com"), skyr::url_parse_error);
  }

  SECTION("path_no_double_slash") {
    auto instance = skyr::url("file:/path/to/something/");
    CHECK("/path/to/something/" == instance.pathname());
  }

  SECTION("path_has_double_slash") {
    auto instance = skyr::url("file:///path/to/something/");
    CHECK("/path/to/something/" == instance.pathname());
  }

  SECTION("url_has_host_bug_87") {
    REQUIRE_THROWS_AS(skyr::url("http://"), skyr::url_parse_error);
  }

  SECTION("url_has_host_bug_87_2") {
    REQUIRE_THROWS_AS(skyr::url("http://user@"), skyr::url_parse_error);
  }

  SECTION("http_scheme_is_special") {
    auto instance = skyr::url("http://example.com");
    REQUIRE(instance.is_special());
  }

  SECTION("https_scheme_is_special") {
    auto instance = skyr::url("http://example.com");
    REQUIRE(instance.is_special());
  }

  SECTION("file_scheme_is_special") {
    auto instance = skyr::url("http://example.com");
    REQUIRE(instance.is_special());
  }

  SECTION("git_scheme_is_not_special") {
    auto instance = skyr::url("git://example.com");
    REQUIRE_FALSE(instance.is_special());
  }

  SECTION("http_default_port_is_80") {
    auto port = skyr::url::default_port("http");
    REQUIRE(port);
    CHECK(80 == port.value());
  }

  SECTION("https_default_port_is_443") {
    auto port = skyr::url::default_port("https");
    REQUIRE(port);
    CHECK(443 == port.value());
  }

  SECTION("file_default_port_doesnt_exist") {
    auto port = skyr::url::default_port("file");
    CHECK_FALSE(port);
  }

  SECTION("git_default_port_is_not_given") {
    auto port = skyr::url::default_port("git");
    CHECK_FALSE(port);
  }

  SECTION("about_blank") {
    auto instance = skyr::url("about:blank");
    CHECK("about:" == instance.protocol());
    CHECK("blank" == instance.pathname());
  }

  SECTION("percent_encoding_fools_parser_1") {
    // https://github.com/glynos/uri/issues/26
    auto instance = skyr::url("http://-error-.invalid/");
    CHECK("http:" == instance.protocol());
    CHECK("-error-.invalid" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("percent_encoding_fools_parser_2") {
    // https://github.com/glynos/uri/issues/26
    auto instance = skyr::url("http://%2Derror-.invalid/");
    CHECK("http:" == instance.protocol());
    CHECK("-error-.invalid" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("percent_encoding_fools_parser_3") {
    // https://github.com/glynos/uri/issues/26
    auto instance = skyr::url("http://xx%2E%2Eyy.invalid/");
    CHECK("http:" == instance.protocol());
    CHECK("xx..yy.invalid" == instance.host());
    CHECK("/" == instance.pathname());
  }

  SECTION("web_platform_tests_1") {
    auto base = skyr::url("http://example.org/foo/bar");
    auto instance = skyr::url("http://foo.com/\\@", base);
    CHECK("//@" == instance.pathname());
  }

  SECTION("web_platform_tests_2") {
    auto base = skyr::url("about:blank");
    auto instance = skyr::url("http://example.com////../..", base);
    CHECK("//" == instance.pathname());
  }

  SECTION("web_platform_tests_3") {
    auto base = skyr::url("about:blank");
    auto instance = skyr::url("sc://faß.ExAmPlE/", base);
    CHECK("fa%C3%9F.ExAmPlE" == instance.host());
  }

  SECTION("web_platform_tests_4") {
    auto base = skyr::url("about:blank");
    auto instance = skyr::url("sc://%/", base);
    CHECK("%" == instance.host());
  }

  SECTION("web_platform_tests_5") {
    auto base = skyr::url("about:blank");
    auto instance = skyr::url("ftp://%e2%98%83", base);
    CHECK("xn--n3h" == instance.host());
  }

  SECTION("web_platform_tests_6") {
    auto base = skyr::url("about:blank");
    auto instance = skyr::url("https://%e2%98%83", base);
    CHECK("xn--n3h" == instance.host());
  }

  SECTION("web_platform_tests_7") {
    auto base = skyr::url("file://host/dir/file");
    auto instance = skyr::url("C|a", base);
    CHECK("host" == instance.host());
  }

  SECTION("web_platform_tests_8") {
    auto base = skyr::url("http://other.com/");
    REQUIRE_THROWS_AS(skyr::url("http://GOO 　goo.com", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_9") {
    auto base = skyr::url("http://other.com/");
    REQUIRE_THROWS_AS(skyr::url(U"http://\xfdD0zyx.com", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_10") {
    auto base = skyr::url("about:blank");
    REQUIRE_THROWS_AS(skyr::url(U"https://\xfffd", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_11") {
    auto base = skyr::url("about:blank");
    REQUIRE_THROWS_AS(skyr::url("http://％００.com", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_12") {
    auto base = skyr::url("http://other.com/");
    REQUIRE_THROWS_AS(skyr::url("http://192.168.0.257", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_13") {
    auto base = skyr::url("http://other.com/");
    REQUIRE_THROWS_AS(skyr::url("http://10000000000", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_14") {
    auto base = skyr::url("http://other.com/");
    REQUIRE_THROWS_AS(skyr::url("http://0xffffffff1", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_15") {
    auto base = skyr::url("http://other.com/");
    REQUIRE_THROWS_AS(skyr::url("http://256.256.256.256", base), skyr::url_parse_error);
  }

  SECTION("web_platform_tests_16") {
    auto base = skyr::url("http://other.com/");
    REQUIRE_THROWS_AS(skyr::url("http://4294967296", base), skyr::url_parse_error);
  }

  SECTION("poo_test") {
    auto instance = skyr::url("http://example.org/\xf0\x9f\x92\xa9");
    CHECK("http:" == instance.protocol());
    CHECK("example.org" == instance.host());
    CHECK("/%F0%9F%92%A9" == instance.pathname());
  }

  SECTION("domain_error_test") {
    auto instance = skyr::make_url(U"http://\xfdD0zyx.com");
    REQUIRE_FALSE(instance);
    CHECK(skyr::url_parse_errc::domain_error == instance.error());
  }

  SECTION("not_an_absolute_url_with_fragment_test") {
    auto instance = skyr::make_url("/\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
    REQUIRE_FALSE(instance);
    CHECK(skyr::url_parse_errc::not_an_absolute_url_with_fragment == instance.error());
  }

  SECTION("pride_flag_test") {
    auto base = skyr::url("https://pride.example/hello-world");
    auto instance = skyr::make_url("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", base);
    REQUIRE(instance);
    CHECK("/%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == instance.value().pathname());
  }

  SECTION("search_parameters_test_1") {
    auto instance = skyr::url("https://example.com/");
    auto search = instance.search_parameters();
    CHECK(search.empty());
    CHECK("" == search.to_string());
    CHECK("" == instance.search());
  }

  SECTION("search_parameters_test_2") {
    auto instance = skyr::url("https://example.com/?");
    auto search = instance.search_parameters();
    CHECK(search.empty());
    CHECK("" == search.to_string());
    CHECK("" == instance.search());
  }

  SECTION("search_parameters_test_3") {
    auto instance = skyr::url("https://example.com/?a=b&c=d");
    auto search = instance.search_parameters();
    CHECK("a=b&c=d" == search.to_string());
    CHECK("?a=b&c=d" == instance.search());
  }

  SECTION("search_parameters_test_4") {
    auto instance = skyr::url("https://example.com/?a=b&c=d");
    auto search = instance.search_parameters();
    search.set("e", "f");
    CHECK("a=b&c=d&e=f" == search.to_string());
    CHECK("?a=b&c=d&e=f" == instance.search());
  }

  SECTION("search_parameters_test_5") {
    auto instance = skyr::url("https://example.com/?a=b&c=d");
    auto search = instance.search_parameters();
    search.set("a", "e");
    CHECK("a=e&c=d" == search.to_string());
    CHECK("?a=e&c=d" == instance.search());
  }

  SECTION("search_parameters_test_6") {
    auto instance = skyr::url("https://example.com/?c=b&a=d");
    auto search = instance.search_parameters();
    search.sort();
    CHECK("a=d&c=b" == search.to_string());
    CHECK("?a=d&c=b" == instance.search());
  }

  SECTION("search_parameters_test_7") {
    auto instance = skyr::url("https://example.com/?c=b&a=d");
    auto search = instance.search_parameters();
    search.clear();
    CHECK(search.empty());
    CHECK("" == search.to_string());
    CHECK("" == instance.search());
  }

  SECTION("url_record_accessor_1") {
    auto instance = skyr::url("https://example.com/?c=b&a=d");
    auto record = instance.record();
  }

  SECTION("url_record_accessor_2") {
    auto record = skyr::url("https://example.com/?c=b&a=d").record();
  }

  SECTION("url_swap") {
    auto url = skyr::url("https://example.com/?a=b&c=d");
    CHECK("https:" == url.protocol());
    CHECK("example.com" == url.host());
    CHECK("?a=b&c=d" == url.search());
    auto instance = skyr::url();
    url.swap(instance);
    CHECK("https:" == instance.protocol());
    CHECK("example.com" == instance.host());
    CHECK("?a=b&c=d" == instance.search());
  }
#endif // __cpp_exceptions
}
