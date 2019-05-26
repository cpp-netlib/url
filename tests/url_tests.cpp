// Copyright 2010 Jeroen Habraken.
// Copyright 2009-2018 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <algorithm>
#include <memory>
#include <map>
#include <set>
#include <unordered_set>
#include <skyr/url.hpp>

TEST(url_tests, construct_invalid_url) {
  EXPECT_THROW(skyr::url("I am not a valid url."), skyr::url_parse_error);
}

TEST(url_tests, construct_invalid_url_make) {
  EXPECT_FALSE(skyr::make_url("I am not a valid url."));
}

TEST(url_tests, construct_url_from_char_array) {
  EXPECT_NO_THROW(skyr::url("http://www.example.com/"));
}

TEST(url_tests, construct_url_from_char_array_make) {
  EXPECT_TRUE(skyr::make_url("http://www.example.com/"));
}

TEST(url_tests, construct_url_starting_with_ipv4_like) {
  EXPECT_NO_THROW(skyr::url("http://198.51.100.0.example.com/"));
}

TEST(url_tests, construct_url_starting_with_ipv4_like_make) {
  EXPECT_TRUE(skyr::make_url("http://198.51.100.0.example.com/"));
}

TEST(url_tests, construct_url_starting_with_ipv4_like_glued) {
  ASSERT_NO_THROW(skyr::url("http://198.51.100.0example.com/"));
}

TEST(url_tests, construct_url_starting_with_ipv4_like_glued_make) {
  ASSERT_TRUE(skyr::make_url("http://198.51.100.0example.com/"));
}

TEST(url_tests, construct_url_like_short_ipv4) {
  EXPECT_NO_THROW(skyr::url("http://198.51.100/"));
}

TEST(url_tests, construct_url_like_short_ipv4_make) {
  EXPECT_TRUE(skyr::make_url("http://198.51.100/"));
}

TEST(url_tests, construct_url_like_long_ipv4) {
  EXPECT_NO_THROW(skyr::url("http://198.51.100.0.255/"));
}

TEST(url_tests, construct_url_like_long_ipv4_make) {
  EXPECT_TRUE(skyr::make_url("http://198.51.100.0.255/"));
}

TEST(url_tests, construct_url_from_string) {
  auto input = std::string("http://www.example.com/");
  EXPECT_NO_THROW((skyr::url(input)));
}

TEST(url_tests, construct_url_from_string_make) {
  auto input = std::string("http://www.example.com/");
  EXPECT_TRUE(skyr::make_url(input));
}

TEST(url_tests, basic_url_protocol_test) {
  auto instance = skyr::url("http://www.example.com/");
  EXPECT_EQ("http:", instance.protocol());
}

TEST(url_tests, basic_url_user_info_test) {
  auto instance = skyr::url("http://www.example.com/");
  EXPECT_TRUE(instance.username().empty());
  EXPECT_TRUE(instance.password().empty());
}

TEST(url_tests, basic_url_host_test) {
  auto instance = skyr::url("http://www.example.com/");
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(url_tests, basic_url_port_test) {
  auto instance = skyr::url("http://www.example.com/");
  EXPECT_TRUE(instance.port().empty());
}

TEST(url_tests, basic_url_path_test) {
  auto instance = skyr::url("http://www.example.com/");
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, basic_url_search_test) {
  auto instance = skyr::url("http://www.example.com/");
  EXPECT_TRUE(instance.search().empty());
}

TEST(url_tests, basic_url_hash_test) {
  auto instance = skyr::url("http://www.example.com/");
  EXPECT_TRUE(instance.hash().empty());
}

TEST(url_tests, full_url_scheme_test) {
  auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("http:", instance.protocol());
}

TEST(url_tests, full_url_user_info_test) {
  auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("user", instance.username());
  EXPECT_TRUE(instance.password().empty());
}

TEST(url_tests, full_url_host_with_default_port_test) {
  auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(url_tests, full_url_hostname_with_default_port_test) {
  auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("www.example.com", instance.hostname());
}

TEST(url_tests, full_url_port_with_default_port_test) {
  auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(instance.port().empty());
}

TEST(url_tests, full_url_host_with_non_default_port_test) {
  auto instance = skyr::url("http://user@www.example.com:8080/path?query#fragment");
  EXPECT_EQ("www.example.com:8080", instance.host());
}

TEST(url_tests, full_url_hostname_with_non_default_port_test) {
  auto instance = skyr::url("http://user@www.example.com:8080/path?query#fragment");
  EXPECT_EQ("www.example.com", instance.hostname());
}

TEST(url_tests, full_url_port_with_non_default_port_test) {
  auto instance = skyr::url("http://user@www.example.com:8080/path?query#fragment");
  EXPECT_EQ("8080", instance.port());
}

TEST(url_tests, full_url_port_as_int_test) {
  auto instance = skyr::url("http://www.example.com:8080/");
  EXPECT_EQ(8080, instance.port<int>());
}

TEST(url_tests, full_url_path_test) {
  auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("/path", instance.pathname());
}

TEST(url_tests, full_url_search_test) {
  auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("?query", instance.search());
}

TEST(url_tests, full_url_hash_test) {
  auto instance = skyr::url("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("#fragment", instance.hash());
}

TEST(url_tests, url_with_empty_search) {
  auto instance = skyr::url("http://example.com/?");
  EXPECT_EQ("", instance.search());
}

TEST(url_tests, mailto_test) {
  auto instance = skyr::url("mailto:john.doe@example.com");
  EXPECT_EQ("mailto:", instance.protocol());
  EXPECT_EQ("john.doe@example.com", instance.pathname());
}

TEST(url_tests, file_test) {
  auto instance = skyr::url("file:///bin/bash");
  EXPECT_EQ("file:", instance.protocol());
  EXPECT_EQ("/bin/bash", instance.pathname());
}

TEST(url_tests, xmpp_test) {
  auto instance = skyr::url("xmpp:example-node@example.com?message;subject=Hello%20World");
  EXPECT_EQ("xmpp:", instance.protocol());
  EXPECT_EQ("example-node@example.com", instance.pathname());
  EXPECT_EQ("?message;subject=Hello%20World", instance.search());
}

TEST(url_tests, ipv4_address_test) {
  auto instance = skyr::url("http://129.79.245.252/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("129.79.245.252", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv4_loopback_test) {
  auto instance = skyr::url("http://127.0.0.1/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("127.0.0.1", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_1) {
  auto instance = skyr::url("http://[1080:0:0:0:8:800:200C:417A]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[1080::8:800:200c:417a]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_2) {
  auto instance = skyr::url("http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[2001:db8:85a3:8d3:1319:8a2e:370:7348]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_3) {
  auto instance = skyr::url("http://[2001:db8:85a3:0:0:8a2e:370:7334]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[2001:db8:85a3::8a2e:370:7334]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_4) {
  auto instance = skyr::url("http://[2001:db8:85a3::8a2e:370:7334]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[2001:db8:85a3::8a2e:370:7334]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_5) {
  auto instance = skyr::url("http://[2001:0db8:0000:0000:0000:0000:1428:57ab]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[2001:db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_6) {
  auto instance = skyr::url("http://[2001:0db8:0000:0000:0000::1428:57ab]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[2001:db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_7) {
  auto instance = skyr::url("http://[2001:0db8:0:0:0:0:1428:57ab]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[2001:db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_8) {
  auto instance = skyr::url("http://[2001:0db8:0:0::1428:57ab]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[2001:db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_9) {
  auto instance = skyr::url("http://[2001:0db8::1428:57ab]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[2001:db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_10) {
  auto instance = skyr::url("http://[2001:db8::1428:57ab]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[2001:db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_11) {
  auto instance = skyr::url("http://[::ffff:0c22:384e]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[::ffff:c22:384e]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_12) {
  auto instance = skyr::url("http://[fe80::]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[fe80::]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_address_test_13) {
  auto instance = skyr::url("http://[::ffff:c000:280]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[::ffff:c000:280]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_loopback_test) {
  auto instance = skyr::url("http://[::1]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[::1]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_loopback_test_1) {
  auto instance = skyr::url("http://[0000:0000:0000:0000:0000:0000:0000:0001]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[::1]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_v4inv6_test_1) {
  auto instance = skyr::url("http://[::ffff:12.34.56.78]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[::ffff:c22:384e]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ipv6_v4inv6_test_2) {
  auto instance = skyr::url("http://[::ffff:192.0.2.128]/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("[::ffff:c000:280]", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, ftp_test) {
  auto instance = skyr::url("ftp://john.doe@ftp.example.com/");
  EXPECT_EQ("ftp:", instance.protocol());
  EXPECT_EQ("john.doe", instance.username());
  EXPECT_EQ("ftp.example.com", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, news_test) {
  auto instance = skyr::url("news:comp.infosystems.www.servers.unix");
  EXPECT_EQ("news:", instance.protocol());
  EXPECT_EQ("comp.infosystems.www.servers.unix", instance.pathname());
}

TEST(url_tests, tel_test) {
  auto instance = skyr::url("tel:+1-816-555-1212");
  EXPECT_EQ("tel:", instance.protocol());
  EXPECT_EQ("+1-816-555-1212", instance.pathname());
}

TEST(url_tests, ldap_test) {
  auto instance = skyr::url("ldap://[2001:db8::7]/c=GB?objectClass?one");
  EXPECT_EQ("ldap:", instance.protocol());
  EXPECT_EQ("[2001:db8::7]", instance.host());
  EXPECT_EQ("/c=GB", instance.pathname());
  EXPECT_EQ("?objectClass?one", instance.search());
}

TEST(url_tests, urn_test) {
  auto instance = skyr::url("urn:oasis:names:specification:docbook:dtd:xml:4.1.2");
  EXPECT_EQ("urn:", instance.protocol());
  EXPECT_EQ("oasis:names:specification:docbook:dtd:xml:4.1.2", instance.pathname());
}

TEST(url_tests, svn_ssh_test) {
  auto instance = skyr::url("svn+ssh://example.com/");
  EXPECT_EQ("svn+ssh:", instance.protocol());
  EXPECT_EQ("example.com", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, range_test) {
  const std::string url("http://www.example.com/");
  auto instance = skyr::url(url);
  EXPECT_TRUE(std::equal(std::begin(instance), std::end(instance), begin(url)));
}

TEST(url_tests, issue_104_test) {
  // https://github.com/cpp-netlib/cpp-netlib/issues/104
  auto instance = std::make_unique<skyr::url>("http://www.example.com/");
  skyr::url copy = *instance;
  instance.reset();
  EXPECT_EQ("http:", copy.protocol());
}

TEST(url_tests, empty_url) {
  auto instance = skyr::url();
  EXPECT_TRUE(instance.empty());
}

TEST(url_tests, whitespace_no_throw) {
  EXPECT_NO_THROW(skyr::url(" http://www.example.com/ "));
}

TEST(url_tests, git) {
  auto instance = skyr::url("git://github.com/cpp-netlib/cpp-netlib.git");
  EXPECT_EQ("git:", instance.protocol());
  EXPECT_EQ("github.com", instance.host());
  EXPECT_EQ("/cpp-netlib/cpp-netlib.git", instance.pathname());
}

TEST(url_tests, invalid_port_test) {
  EXPECT_THROW(skyr::url("http://123.34.23.56:6662626/"), skyr::url_parse_error);
}

TEST(url_tests, valid_empty_port_test) {
  EXPECT_NO_THROW(skyr::url("http://123.34.23.56:/"));
}

TEST(url_tests, empty_port_test) {
  auto instance = skyr::url("http://123.34.23.56:/");
  EXPECT_EQ("", instance.port());
}

TEST(url_tests, nonspecial_url_with_one_slash) {
  EXPECT_NO_THROW(skyr::url("scheme:/path/"));
}

TEST(url_tests, url_begins_with_a_colon) {
  EXPECT_THROW(skyr::url("://example.com"), skyr::url_parse_error);
}

TEST(url_tests, url_begins_with_a_number) {
  EXPECT_THROW(skyr::url("3http://example.com"), skyr::url_parse_error);
}

TEST(url_tests, url_scheme_contains_an_invalid_character) {
  EXPECT_THROW(skyr::url("ht%tp://example.com"), skyr::url_parse_error);
}

TEST(url_tests, path_no_double_slash) {
  auto instance = skyr::url("file:/path/to/something/");
  EXPECT_EQ("/path/to/something/", instance.pathname());
}

TEST(url_tests, path_has_double_slash) {
  auto instance = skyr::url("file:///path/to/something/");
  EXPECT_EQ("/path/to/something/", instance.pathname());
}

TEST(url_tests, url_has_host_bug_87) {
  EXPECT_THROW(skyr::url("http://"), skyr::url_parse_error);
}

TEST(url_tests, url_has_host_bug_87_2) {
  EXPECT_THROW(skyr::url("http://user@"), skyr::url_parse_error);
}

TEST(url_tests, http_scheme_is_special) {
  auto instance = skyr::url("http://example.com");
  ASSERT_TRUE(instance.is_special());
}

TEST(url_tests, https_scheme_is_special) {
  auto instance = skyr::url("http://example.com");
  ASSERT_TRUE(instance.is_special());
}

TEST(url_tests, file_scheme_is_special) {
  auto instance = skyr::url("http://example.com");
  ASSERT_TRUE(instance.is_special());
}

TEST(url_tests, git_scheme_is_not_special) {
  auto instance = skyr::url("git://example.com");
  ASSERT_FALSE(instance.is_special());
}

TEST(url_tests, http_default_port_is_80) {
  auto port = skyr::url::default_port("http");
  ASSERT_TRUE(port);
  ASSERT_EQ(80, port.value());
}

TEST(url_tests, https_default_port_is_443) {
  auto port = skyr::url::default_port("https");
  ASSERT_TRUE(port);
  ASSERT_EQ(443, port.value());
}

TEST(url_tests, file_default_port_doesnt_exist) {
  auto port = skyr::url::default_port("file");
  ASSERT_FALSE(port);
}

TEST(url_tests, git_default_port_is_not_given) {
  auto port = skyr::url::default_port("git");
  ASSERT_FALSE(port);
}

TEST(url_tests, about_blank) {
  auto instance = skyr::url("about:blank");
  EXPECT_EQ("about:", instance.protocol());
  EXPECT_EQ("blank", instance.pathname());
}

TEST(url_tests, percent_encoding_fools_parser_1) {
  // https://github.com/glynos/uri/issues/26
  auto instance = skyr::url("http://-error-.invalid/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("-error-.invalid", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, percent_encoding_fools_parser_2) {
  // https://github.com/glynos/uri/issues/26
  auto instance = skyr::url("http://%2Derror-.invalid/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("-error-.invalid", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, percent_encoding_fools_parser_3) {
  // https://github.com/glynos/uri/issues/26
  auto instance = skyr::url("http://xx%2E%2Eyy.invalid/");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("xx..yy.invalid", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_tests, web_platform_tests_1) {
  auto base = skyr::url("http://example.org/foo/bar");
  auto instance = skyr::url("http://foo.com/\\@", base);
  EXPECT_EQ("//@", instance.pathname());
}

TEST(url_tests, web_platform_tests_2) {
  auto base = skyr::url("about:blank");
  auto instance = skyr::url("http://example.com////../..", base);
  EXPECT_EQ("//", instance.pathname());
}

TEST(url_tests, web_platform_tests_3) {
  auto base = skyr::url("about:blank");
  auto instance = skyr::url("sc://faß.ExAmPlE/", base);
  EXPECT_EQ("fa%C3%9F.ExAmPlE", instance.host());
}

TEST(url_tests, web_platform_tests_4) {
  auto base = skyr::url("about:blank");
  auto instance = skyr::url("sc://%/", base);
  EXPECT_EQ("%", instance.host());
}

TEST(url_tests, web_platform_tests_5) {
  auto base = skyr::url("about:blank");
  auto instance = skyr::url("ftp://%e2%98%83", base);
  EXPECT_EQ("xn--n3h", instance.host());
}

TEST(url_tests, web_platform_tests_6) {
  auto base = skyr::url("about:blank");
  auto instance = skyr::url("https://%e2%98%83", base);
  EXPECT_EQ("xn--n3h", instance.host());
}

TEST(url_tests, web_platform_tests_7) {
  auto base = skyr::url("file://host/dir/file");
  auto instance = skyr::url("C|a", base);
  EXPECT_EQ("host", instance.host());
}

TEST(url_tests, web_platform_tests_8) {
  auto base = skyr::url("http://other.com/");
  ASSERT_THROW(skyr::url("http://GOO 　goo.com", base), skyr::url_parse_error);
}

TEST(url_tests, web_platform_tests_9) {
  auto base = skyr::url("http://other.com/");
  ASSERT_THROW(skyr::url(U"http://\xfdD0zyx.com", base), skyr::url_parse_error);
}

TEST(url_tests, web_platform_tests_10) {
  auto base = skyr::url("about:blank");
  ASSERT_THROW(skyr::url(U"https://\xfffd", base), skyr::url_parse_error);
}

TEST(url_tests, web_platform_tests_11) {
  auto base = skyr::url("about:blank");
  ASSERT_THROW(skyr::url("http://％００.com", base), skyr::url_parse_error);
}

TEST(url_tests, web_platform_tests_12) {
  auto base = skyr::url("http://other.com/");
  ASSERT_THROW(skyr::url("http://192.168.0.257", base), skyr::url_parse_error);
}

TEST(url_tests, web_platform_tests_13) {
  auto base = skyr::url("http://other.com/");
  ASSERT_THROW(skyr::url("http://10000000000", base), skyr::url_parse_error);
}

TEST(url_tests, web_platform_tests_14) {
  auto base = skyr::url("http://other.com/");
  ASSERT_THROW(skyr::url("http://0xffffffff1", base), skyr::url_parse_error);
}

TEST(url_tests, web_platform_tests_15) {
  auto base = skyr::url("http://other.com/");
  ASSERT_THROW(skyr::url("http://256.256.256.256", base), skyr::url_parse_error);
}

TEST(url_tests, web_platform_tests_16) {
  auto base = skyr::url("http://other.com/");
  ASSERT_THROW(skyr::url("http://4294967296", base), skyr::url_parse_error);
}

TEST(url_test, poo_test) {
  auto instance = skyr::url("http://example.org/\xf0\x9f\x92\xa9");
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("example.org", instance.host());
  EXPECT_EQ("/%F0%9F%92%A9", instance.pathname());
}

TEST(url_test, domain_error_test) {
  auto instance = skyr::make_url(U"http://\xfdD0zyx.com");
  ASSERT_FALSE(instance);
  EXPECT_EQ(skyr::url_parse_errc::domain_error, instance.error());
}

TEST(url_test, not_an_absolute_url_with_fragment_test) {
  auto instance = skyr::make_url("/\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
  ASSERT_FALSE(instance);
  EXPECT_EQ(skyr::url_parse_errc::not_an_absolute_url_with_fragment, instance.error());
}

TEST(url_test, pride_flag_test) {
  auto base = skyr::url("https://pride.example/hello-world");
  auto instance = skyr::make_url("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", base);
  ASSERT_TRUE(instance);
  EXPECT_EQ("/%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88", instance.value().pathname());
}

TEST(url_test, search_parameters_test_1) {
  auto instance = skyr::url("https://example.com/");
  auto search = instance.search_parameters();
  EXPECT_TRUE(search.empty());
  EXPECT_EQ("", search.to_string());
  EXPECT_EQ("", instance.search());
}

TEST(url_test, search_parameters_test_2) {
  auto instance = skyr::url("https://example.com/?");
  auto search = instance.search_parameters();
  EXPECT_TRUE(search.empty());
  EXPECT_EQ("", search.to_string());
  EXPECT_EQ("", instance.search());
}

TEST(url_test, search_parameters_test_3) {
  auto instance = skyr::url("https://example.com/?a=b&c=d");
  auto search = instance.search_parameters();
  EXPECT_EQ("a=b&c=d", search.to_string());
  EXPECT_EQ("?a=b&c=d", instance.search());
}

TEST(url_test, search_parameters_test_4) {
  auto instance = skyr::url("https://example.com/?a=b&c=d");
  auto search = instance.search_parameters();
  search.set("e", "f");
  EXPECT_EQ("a=b&c=d&e=f", search.to_string());
  EXPECT_EQ("?a=b&c=d&e=f", instance.search());
}

TEST(url_test, search_parameters_test_5) {
  auto instance = skyr::url("https://example.com/?a=b&c=d");
  auto search = instance.search_parameters();
  search.set("a", "e");
  EXPECT_EQ("a=e&c=d", search.to_string());
  EXPECT_EQ("?a=e&c=d", instance.search());
}

TEST(url_test, search_parameters_test_6) {
  auto instance = skyr::url("https://example.com/?c=b&a=d");
  auto search = instance.search_parameters();
  search.sort();
  EXPECT_EQ("a=d&c=b", search.to_string());
  EXPECT_EQ("?a=d&c=b", instance.search());
}

TEST(url_test, search_parameters_test_7) {
  auto instance = skyr::url("https://example.com/?c=b&a=d");
  auto search = instance.search_parameters();
  search.clear();
  EXPECT_TRUE(search.empty());
  EXPECT_EQ("", search.to_string());
  EXPECT_EQ("", instance.search());
}

TEST(url_test, url_record_accessor_1) {
  auto instance = skyr::url("https://example.com/?c=b&a=d");
  auto record = instance.record();
}

TEST(url_test, url_record_accessor_2) {
  auto record = skyr::url("https://example.com/?c=b&a=d").record();
}

TEST(url_test, url_swap) {
  auto url = skyr::url("https://example.com/?a=b&c=d");
  EXPECT_EQ("https:", url.protocol());
  EXPECT_EQ("example.com", url.host());
  EXPECT_EQ("?a=b&c=d", url.search());
  auto instance = skyr::url();
  url.swap(instance);
  EXPECT_EQ("https:", instance.protocol());
  EXPECT_EQ("example.com", instance.host());
  EXPECT_EQ("?a=b&c=d", instance.search());
}
