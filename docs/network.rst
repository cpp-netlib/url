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
   #include <iostream>
   #include <cassert>

   int main() {
     using namespace skyr::literals;

     auto url = "http://[1080:0:0:0:8:800:200C:417A]:8090/"_url;
     assert(url.is_ipv6_address());
     std::cout << "IPv6 Host: " << urlhostname() << std::endl;
   }

API
---

IPv4
^^^^

.. doxygenclass:: skyr::v1::ipv4_address
    :members:

IPv6
^^^^

.. doxygenclass:: skyr::v1::ipv6_address
    :members:
