JSON
====

Description
-----------

The functions below convert between a URL query string and a JSON
object. They depend on the
`nlohmann <https://github.com/nlohmann/json>`_ library.

Headers
-------

.. code-block:: c++

    #include <skyr/json/json.hpp>

Example
-------

API
---

JSON
^^^^

.. doxygenfunction:: skyr::json::encode_query

.. doxygenfunction:: skyr::json::decode_query
