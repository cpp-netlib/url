Domain Name Processing
======================

Description
-----------

Punycode is an encoding which represents Unicode in the ASCII
character set. It used for Internet domain names. It is specified
in `RFC 3482 <https://www.ietf.org/rfc/rfc3492.txt>`_.

The functions below apply conversions between Unicode encoded
domain names and ASCII.

Headers
-------

.. code-block:: c++

   #include <skyr/domain.hpp>

Example
-------

.. code-block:: c++

   #include <skyr/domain.hpp>
   #include <print>

   int main() {
     using namespace std::string_literals;

     auto domain = "उदाहरण.परीक्षा"s;
     auto encoded = skyr::domain::domain_to_ascii(domain);
     if (encoded) {
       std::println(encoded.value());
     }
   }

API
---

Domain Processing Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The library provides the following domain processing functions. See the header files for detailed documentation of all overloads.

Error codes
^^^^^^^^^^^

.. doxygenenum:: skyr::domain_errc
