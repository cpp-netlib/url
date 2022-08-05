// Copyright 2010 Jeroen Habraken.
// Copyright 2009-2020 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <memory>
#include <catch2/catch_all.hpp>
#include <skyr/v1/url.hpp>

TEST_CASE("url_tests", "[url]") {
  using namespace std::string_literals;

  SECTION("construct_invalid_url_make") {
    CHECK_FALSE(skyr::make_url("I am not a valid url."));
  }

  SECTION("construct_url_from_char_array_make") {
    CHECK(skyr::make_url("http://www.example.com/"));
  }

  SECTION("construct_url_starting_with_ipv4_like_make") {
    CHECK(skyr::make_url("http://198.51.100.0.example.com/"));
  }

  SECTION("construct_url_starting_with_ipv4_like_glued_make") {
    CHECK(skyr::make_url("http://198.51.100.0example.com/"));
  }

  SECTION("construct_url_like_short_ipv4_make") {
    CHECK(skyr::make_url("http://198.51.100/"));
  }

  SECTION("construct_url_like_long_ipv4_make)") {
    CHECK(skyr::make_url("http://198.51.100.0.255/"));
  }

  SECTION("construct_url_from_string_make") {
    const auto input = "http://www.example.com/"s;
    CHECK(skyr::make_url(input));
  }

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
    CHECK(instance.is_ipv4_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv4_loopback_test") {
    auto instance = skyr::url("http://127.0.0.1/");
    CHECK("http:" == instance.protocol());
    CHECK("127.0.0.1" == instance.host());
    CHECK(instance.is_ipv4_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_1") {
    auto instance = skyr::url("http://[1080:0:0:0:8:800:200C:417A]/");
    CHECK("http:" == instance.protocol());
    CHECK("[1080::8:800:200c:417a]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_2") {
    auto instance = skyr::url("http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8:85a3:8d3:1319:8a2e:370:7348]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_3") {
    auto instance = skyr::url("http://[2001:db8:85a3:0:0:8a2e:370:7334]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8:85a3::8a2e:370:7334]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_4") {
    auto instance = skyr::url("http://[2001:db8:85a3::8a2e:370:7334]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8:85a3::8a2e:370:7334]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_5") {
    auto instance = skyr::url("http://[2001:0db8:0000:0000:0000:0000:1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_6") {
    auto instance = skyr::url("http://[2001:0db8:0000:0000:0000::1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_7") {
    auto instance = skyr::url("http://[2001:0db8:0:0:0:0:1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_8") {
    auto instance = skyr::url("http://[2001:0db8:0:0::1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_9") {
    auto instance = skyr::url("http://[2001:0db8::1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_10") {
    auto instance = skyr::url("http://[2001:db8::1428:57ab]/");
    CHECK("http:" == instance.protocol());
    CHECK("[2001:db8::1428:57ab]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_11") {
    auto instance = skyr::url("http://[::ffff:0c22:384e]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::ffff:c22:384e]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_12") {
    auto instance = skyr::url("http://[fe80::]/");
    CHECK("http:" == instance.protocol());
    CHECK("[fe80::]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_address_test_13") {
    auto instance = skyr::url("http://[::ffff:c000:280]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::ffff:c000:280]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_loopback_test") {
    auto instance = skyr::url("http://[::1]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::1]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_loopback_test_1") {
    auto instance = skyr::url("http://[0000:0000:0000:0000:0000:0000:0000:0001]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::1]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_v4inv6_test_1") {
    auto instance = skyr::url("http://[::ffff:12.34.56.78]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::ffff:c22:384e]" == instance.host());
    CHECK(instance.is_ipv6_address());
    CHECK("/" == instance.pathname());
  }

  SECTION("ipv6_v4inv6_test_2") {
    auto instance = skyr::url("http://[::ffff:192.0.2.128]/");
    CHECK("http:" == instance.protocol());
    CHECK("[::ffff:c000:280]" == instance.host());
    CHECK(instance.is_ipv6_address());
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

  SECTION("git") {
    auto instance = skyr::url("git://github.com/cpp-netlib/cpp-netlib.git");
    CHECK("git:" == instance.protocol());
    CHECK("github.com" == instance.host());
    CHECK("/cpp-netlib/cpp-netlib.git" == instance.pathname());
  }

  SECTION("empty_port_test") {
    auto instance = skyr::url("http://123.34.23.56:/");
    CHECK("" == instance.port());
  }

  SECTION("path_no_double_slash") {
    auto instance = skyr::url("file:/path/to/something/");
    CHECK("/path/to/something/" == instance.pathname());
  }

  SECTION("path_has_double_slash") {
    auto instance = skyr::url("file:///path/to/something/");
    CHECK("/path/to/something/" == instance.pathname());
  }

  SECTION("http_scheme_is_special") {
    auto instance = skyr::url("http://example.com");
    CHECK(instance.is_special());
  }

  SECTION("https_scheme_is_special") {
    auto instance = skyr::url("https://example.com");
    CHECK(instance.is_special());
  }

  SECTION("file_scheme_is_special") {
    auto instance = skyr::url("file://example.com");
    CHECK(instance.is_special());
  }

  SECTION("git_scheme_is_not_special") {
    auto instance = skyr::url("git://example.com");
    CHECK_FALSE(instance.is_special());
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

  SECTION("http_default_port_is_80_using_protocol") {
    auto port = skyr::url::default_port("http:");
    REQUIRE(port);
    CHECK(80 == port.value());
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

  SECTION("poo_test") {
    auto instance = skyr::url("http://example.org/\xf0\x9f\x92\xa9");
    CHECK("http:" == instance.protocol());
    CHECK("example.org" == instance.host());
    CHECK("/%F0%9F%92%A9" == instance.pathname());
  }

  SECTION("domain_error_test") {
    auto t = std::u32string(U"\xfdD0");
    auto byte = skyr::details::to_u8(t);
    CHECK(byte);

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

  SECTION("pride_flag_test_from_u32") {
    auto base = skyr::url("https://pride.example/hello-world");
    auto instance = skyr::make_url(U"\x1F3F3\xFE0F\x200D\x1F308", base);
    REQUIRE(instance);
    CHECK("/%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88" == instance.value().pathname());
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

  SECTION("url_copy_assign") {
    auto url = skyr::url("https://example.com/?a=b&c=d");
    auto instance = skyr::url();
    instance = url;
    CHECK("https:" == instance.protocol());
    CHECK("example.com" == instance.host());
    CHECK("?a=b&c=d" == instance.search());
  }

  SECTION("url_move_assign") {
    auto url = skyr::url("https://example.com/?a=b&c=d");
    auto instance = skyr::url();
    instance = std::move(url);
    CHECK("https:" == instance.protocol());
    CHECK("example.com" == instance.host());
    CHECK("?a=b&c=d" == instance.search());
  }

  SECTION("domain_to_ascii_be_strict_issue_36") {
    auto instance = skyr::url("https://+:80/vroot/");
    CHECK("https:" == instance.protocol());
    CHECK("+:80" == instance.host());
    CHECK("+" == instance.hostname());
    CHECK("80" == instance.port());
    CHECK("/vroot/" == instance.pathname());
  }

  SECTION("is_ipv4_accessor_like_short_ipv4_issue_51") {
    auto instance = skyr::url("http://198.51.100/");
    CHECK(instance.is_ipv4_address());
    CHECK(!instance.is_ipv6_address());
    CHECK(!instance.is_domain());
    CHECK(!instance.is_opaque_host());
  }

  SECTION("is_ipv4_accessor_like_long_ipv4_issue_51") {
    auto instance = skyr::url("http://198.51.100.0.255/");
    CHECK(!instance.is_ipv4_address());
    CHECK(!instance.is_ipv6_address());
    CHECK(instance.is_domain());
    CHECK(!instance.is_opaque_host());
  }

  SECTION("is_ipv4_accessor_like_short_ipv4_issue_51") {
    auto instance = skyr::url("http://0x7f.0.0.0x7f/");
    CHECK(instance.is_ipv4_address());
    CHECK(!instance.is_ipv6_address());
    CHECK(!instance.is_domain());
    CHECK(!instance.is_opaque_host());
  }

  SECTION("is_domain_issue_51") {
    auto instance = skyr::url("http://www.example.com/");
    CHECK(!instance.is_ipv4_address());
    CHECK(!instance.is_ipv6_address());
    CHECK(instance.is_domain());
    CHECK(!instance.is_opaque_host());
  }

  SECTION("is_opaque_issue_5") {
    auto instance = skyr::url("git://example.com");
    CHECK(!instance.is_ipv4_address());
    CHECK(!instance.is_ipv6_address());
    CHECK(!instance.is_domain());
    CHECK(instance.is_opaque_host());
  }

  SECTION("is_opaque_ipv6_issue_5") {
    auto instance = skyr::url("non-special://[1:2:0:0:5:0:0:0]/");
    CHECK(!instance.is_ipv4_address());
    CHECK(instance.is_ipv6_address());
    CHECK(!instance.is_domain());
    CHECK(!instance.is_opaque_host());
  }

  SECTION("is_opaque_ipv6_issue_6") {
    auto instance = skyr::url("non-special://example.com/");
    CHECK(!instance.is_ipv4_address());
    CHECK(!instance.is_ipv6_address());
    CHECK(!instance.is_domain());
    CHECK(instance.is_opaque_host());
  }

  SECTION("url_origin_01") {
    auto instance = skyr::url("https://example.org/foo/bar?baz");
    CHECK("https://example.org" == instance.origin());
  }

  SECTION("url_origin_02") {
    auto instance = skyr::url("https://測試");
    CHECK("https://xn--g6w251d" == instance.origin());
  }

  SECTION("url_origin_03") {
    auto instance = skyr::url("blob:https://whatwg.org/d0360e2f-caee-469f-9a2f-87d5b0456f6f");
    CHECK("https://whatwg.org" == instance.origin());
  }

  SECTION("url_domain") {
    auto instance = skyr::url("https://xn--p1b6ci4b4b3a.xn--11b5bs3a9aj6g/path/");
    CHECK(instance.is_domain());
    CHECK(instance.domain().value() == "xn--p1b6ci4b4b3a.xn--11b5bs3a9aj6g");
    CHECK(instance.u8domain().value() == "उदाहरण.परीक्षा");
  }

  SECTION("regression_failure_01") {
    auto base = skyr::url("file:///tmp/mock/path");
    auto instance = skyr::url("file:c:\\foo\\bar.html", base);
    CHECK(instance.protocol() == "file:");
    CHECK(instance.pathname() == "/c:/foo/bar.html");
  }

  SECTION("regression_failure_02") {
    auto base = skyr::url("about:blank");
    auto instance = skyr::url("http://example.org/test?%GH", base);
    CHECK(instance.search() == "?%GH");
  }

  SECTION("regression_failure_03") {
    auto instance = skyr::url("http://./");
    CHECK(instance.href() == "http://./");
  }

  SECTION("regression_failure_04") {
    auto instance = skyr::url("http://../");
    CHECK(instance.href() == "http://../");
  }

  SECTION("null_code_point_in_fragment") {
    auto instance = skyr::url(U"http://example.org/test?a#b\u0000c");
    CHECK(instance.href() == "http://example.org/test?a#b%00c");
    CHECK(instance.hash() == "#b%00c");
  }

  SECTION("windows_drive_letter_quirk_01") {
    auto instance = skyr::url("file://1.2.3.4/C:/");
    CHECK(instance.href() == "file:///C:/");
    CHECK(instance.pathname() == "/C:/");
  }

  SECTION("windows_drive_letter_quirk_02") {
    auto instance = skyr::url("file://[1::8]/C:/");
    CHECK(instance.href() == "file:///C:/");
    CHECK(instance.pathname() == "/C:/");
  }
}
