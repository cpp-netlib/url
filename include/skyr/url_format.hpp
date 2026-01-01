// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_FORMAT_HPP
#define SKYR_URL_FORMAT_HPP

#include <format>

#include <skyr/percent_encoding/percent_decode.hpp>
#include <skyr/url.hpp>

/// \file url_format.hpp
/// Provides std::format support for skyr::url
///
/// Format specifications:
/// - {} or {:}  - Full URL (default)
/// - {:s}       - Scheme (without ':')
/// - {:h}       - Hostname (punycode/ASCII)
/// - {:hd}      - Hostname (unicode decoded)
/// - {:p}       - Port (empty if default)
/// - {:P}       - Pathname (percent-encoded)
/// - {:Pd}      - Pathname (percent-decoded)
/// - {:q}       - Search/query (percent-encoded, with '?')
/// - {:qd}      - Search/query (percent-decoded)
/// - {:f}       - Fragment (percent-encoded, with '#')
/// - {:fd}      - Fragment (percent-decoded)
/// - {:o}       - Origin (scheme://host:port)
///
/// The 'd' modifier decodes percent-encoding and punycode.
/// If decoding fails, falls back to the encoded version.
///
/// Examples:
/// \code
/// auto url = skyr::url("http://example.إختبار/π?name=John%20Doe");
/// std::println("{:h}", url);    // example.xn--kgbechtv (punycode)
/// std::println("{:hd}", url);   // example.إختبار (unicode)
/// std::println("{:P}", url);    // /%CF%80 (encoded)
/// std::println("{:Pd}", url);   // /π (decoded)
/// std::println("{:q}", url);    // ?name=John%20Doe (encoded)
/// std::println("{:qd}", url);   // ?name=John Doe (decoded)
/// \endcode

template <>
struct std::formatter<skyr::url> {
  enum class format_type {
    full,      // Full URL (default)
    scheme,    // s - scheme
    hostname,  // h - hostname
    port,      // p - port
    pathname,  // P - pathname
    query,     // q - query/search
    fragment,  // f - fragment
    origin     // o - origin
  };

  format_type type_ = format_type::full;
  bool decode_ = false;  // 'd' modifier for decoded output

  constexpr auto parse(std::format_parse_context& ctx) {
    auto it = ctx.begin();
    const auto end = ctx.end();

    if (it == end || *it == '}') {
      type_ = format_type::full;
      decode_ = false;
      return it;
    }

    // Parse format spec
    switch (*it) {
      case 's':
        type_ = format_type::scheme;
        ++it;
        break;
      case 'h':
        type_ = format_type::hostname;
        ++it;
        break;
      case 'p':
        type_ = format_type::port;
        ++it;
        break;
      case 'P':
        type_ = format_type::pathname;
        ++it;
        break;
      case 'q':
        type_ = format_type::query;
        ++it;
        break;
      case 'f':
        type_ = format_type::fragment;
        ++it;
        break;
      case 'o':
        type_ = format_type::origin;
        ++it;
        break;
      default:
        throw std::format_error("Invalid format specifier for skyr::url");
    }

    // Check for 'd' (decode) modifier
    if (it != end && *it == 'd') {
      decode_ = true;
      ++it;
    }

    if (it != end && *it != '}') {
      throw std::format_error("Invalid format specifier for skyr::url");
    }

    return it;
  }

  auto format(const skyr::url& url, std::format_context& ctx) const {
    switch (type_) {
      case format_type::full:
        return std::format_to(ctx.out(), "{}", url.href());

      case format_type::scheme:
        return std::format_to(ctx.out(), "{}", url.scheme());

      case format_type::hostname:
        if (decode_) {
          // Try to get unicode domain, fall back to ASCII if not available
          if (auto domain = url.u8domain()) {
            return std::format_to(ctx.out(), "{}", domain.value());
          }
        }
        return std::format_to(ctx.out(), "{}", url.hostname());

      case format_type::port:
        return std::format_to(ctx.out(), "{}", url.port());

      case format_type::pathname: {
        auto pathname = url.pathname();
        if (decode_) {
          // Try to percent-decode, fall back to encoded if decode fails
          if (auto decoded = skyr::percent_decode(pathname)) {
            return std::format_to(ctx.out(), "{}", decoded.value());
          }
        }
        return std::format_to(ctx.out(), "{}", pathname);
      }

      case format_type::query: {
        auto search = url.search();
        if (decode_ && !search.empty()) {
          // Decode the query string (skip the leading '?')
          auto query_part = search.substr(1);  // Remove '?'
          if (auto decoded = skyr::percent_decode(query_part)) {
            return std::format_to(ctx.out(), "?{}", decoded.value());
          }
        }
        return std::format_to(ctx.out(), "{}", search);
      }

      case format_type::fragment: {
        auto hash = url.hash();
        if (decode_ && !hash.empty()) {
          // Decode the fragment (skip the leading '#')
          auto fragment_part = hash.substr(1);  // Remove '#'
          if (auto decoded = skyr::percent_decode(fragment_part)) {
            return std::format_to(ctx.out(), "#{}", decoded.value());
          }
        }
        return std::format_to(ctx.out(), "{}", hash);
      }

      case format_type::origin:
        return std::format_to(ctx.out(), "{}", url.origin());

      default:
        return ctx.out();
    }
  }
};

#endif  // SKYR_URL_FORMAT_HPP
