Network Addresses
=================

Description
-----------

Two classes representing IPv4 and IPv6 addresses are provided.

Headers
-------

.. code-block:: c++

    #include <skyr/network/ipv4_address.hpp>
    #include <skyr/network/ipv6_address.hpp>

Example
-------

.. code-block:: c++

   #include <skyr/url.hpp>
   #include <print>
   #include <cassert>

   int main() {
     using namespace skyr::literals;

     auto url = "http://[1080:0:0:0:8:800:200C:417A]:8090/"_url;
     assert(url.is_ipv6_address());
     std::println("IPv6 Host: {}", url.hostname());
   }

API
---

IPv4
^^^^

.. doxygenclass:: skyr::ipv4_address
    :members:

IPv6
^^^^

.. doxygenclass:: skyr::ipv6_address
    :members:
