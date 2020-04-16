Percent Encoding
================

Description
-----------

The functions below allow percent encoding and decoding, most
commonly used in URL paths, query parameter and fragments.

Headers
-------

.. code-block:: c++

    #include <skyr/percent_encoding/percent_encode.hpp>
    #include <skyr/percent_encoding/percent_decode.hpp>

Example
-------

.. code-block:: c++

   #include <skyr/url.hpp>
   #include <skyr/percent_encoding/percent_decode.hpp>
   #include <iostream>

   int main() {
     using namespace skyr::literals;

     auto url = "http://www.example.org/\xcf\x80/"_url;
     std::cout << skyr::percent_decode<std::string>(url.pathname()).value() << std::endl;
   }

API
---

Percent encoding
^^^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::percent_encode

.. doxygenfunction:: skyr::percent_decode
