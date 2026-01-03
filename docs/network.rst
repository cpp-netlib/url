Network Addresses
=================

Description
-----------

The library provides two classes representing IPv4 and IPv6 addresses
according to the WhatWG URL specification. These addresses are parsed
and validated as part of URL hostname processing.

Headers
-------

.. code-block:: c++

    #include <skyr/network/ipv4_address.hpp>
    #include <skyr/network/ipv6_address.hpp>

Example
-------

.. code-block:: c++

   #include <skyr/url.hpp>
   #include <skyr/network/ipv4_address.hpp>
   #include <skyr/network/ipv6_address.hpp>
   #include <print>

   int main() {
     using namespace skyr::literals;

     // IPv4 addresses
     auto ipv4_url = "http://192.168.1.1:8080/api"_url;
     if (ipv4_url.is_ipv4_address()) {
       std::println("IPv4 Host: {}", ipv4_url.hostname());
       // Output: 192.168.1.1
     }

     // IPv6 addresses (must be enclosed in brackets)
     auto ipv6_url = "http://[1080:0:0:0:8:800:200C:417A]:8090/"_url;
     if (ipv6_url.is_ipv6_address()) {
       std::println("IPv6 Host: {}", ipv6_url.hostname());
       // Output: [1080:0:0:0:8:800:200c:417a]
     }

     // Parse IPv4 address directly
     auto ipv4 = skyr::parse_ipv4_address("192.0.2.1");
     if (ipv4) {
       std::println("Parsed IPv4: {}", ipv4->serialize());
       // Output: 192.0.2.1
     }

     // Parse IPv6 address directly
     auto ipv6 = skyr::parse_ipv6_address("2001:db8::1");
     if (ipv6) {
       std::println("Parsed IPv6: {}", ipv6->serialize());
       // Output: 2001:db8::1
     }
   }

API
---

IPv4 Address
^^^^^^^^^^^^

.. doxygenclass:: skyr::ipv4_address
    :members:

.. doxygenfunction:: skyr::parse_ipv4_address(std::string_view, bool*)

IPv6 Address
^^^^^^^^^^^^

.. doxygenclass:: skyr::ipv6_address
    :members:

.. doxygenfunction:: skyr::parse_ipv6_address(std::string_view, bool*)

Error Codes
^^^^^^^^^^^

.. doxygenenum:: skyr::ipv4_address_errc

.. doxygenenum:: skyr::ipv6_address_errc
