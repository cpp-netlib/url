Percent Encoding
================

Description
-----------

The functions below allow percent encoding and decoding, commonly
used in URL components such as paths, query parameters, and fragments.

Percent encoding converts characters that are not allowed in URLs
into a ``%`` followed by two hexadecimal digits representing the
character's byte value in UTF-8.

Headers
-------

.. code-block:: c++

   #include <skyr/percent_encoding.hpp>

Example
-------

.. code-block:: c++

   #include <skyr/percent_encoding.hpp>
   #include <print>

   int main() {
     // Encode a string with special characters
     auto encoded = skyr::percent_encode("Hello World! π = 3.14");
     std::println("Encoded: {}", encoded);
     // Output: Hello%20World!%20%CF%80%20%3D%203.14

     // Decode a percent-encoded string
     auto decoded = skyr::percent_decode("Hello%20World!%20%CF%80%20%3D%203.14");
     if (decoded) {
       std::println("Decoded: {}", decoded.value());
       // Output: Hello World! π = 3.14
     }

     // Using with URL components
     auto path_component = "path/to/file with spaces.txt";
     auto encoded_path = skyr::percent_encode(path_component);
     std::println("Encoded path: {}", encoded_path);
     // Output: path%2Fto%2Ffile%20with%20spaces.txt

     // Common use case: building query parameters
     auto key = "search query";
     auto value = "C++ & Rust";
     auto query = std::format("{}={}",
                               skyr::percent_encode(key),
                               skyr::percent_encode(value));
     std::println("Query: {}", query);
     // Output: search%20query=C%2B%2B%20%26%20Rust
   }

API
---

Percent Encoding Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::percent_encode

.. doxygenfunction:: skyr::percent_decode

Error Codes
^^^^^^^^^^^

.. doxygenenum:: skyr::percent_encoding::percent_encode_errc
