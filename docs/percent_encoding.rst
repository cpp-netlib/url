Percent Encoding
================

Description
-----------

Headers
-------

.. code-block:: c++

    #include <skyr/percent_encoding/percent_encode.hpp>
    #include <skyr/percent_encoding/percent_decode.hpp>

Example
-------

The functions below allow percent encoding and decoding, most
commonly used in URL paths, query parameter and fragments.

API
---

Percent encoding
^^^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::percent_encode

.. doxygenfunction:: skyr::percent_decode
