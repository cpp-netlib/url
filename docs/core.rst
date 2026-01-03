Core
====

Description
-----------

The core of the ``skyr`` library is a ``skyr::parse`` function
that parses an input string and creates a ``skyr::url_record``
that contains URL parts, and a ``skyr::serialize`` function
that composes a URL string from an existing ``skyr::url_record``.

The ``skyr::parse`` function returns ``std::expected<url_record, url_parse_errc>``
for type-safe error handling without exceptions.

Use these functions directly if your needs are simple. Use the
``skyr::url`` class for more complex operations, including
Unicode encoding and decoding, URL transformations, and sanitization.

Headers
-------

.. code-block:: c++

    #include <skyr/core/url_record.hpp>
    #include <skyr/core/parse.hpp>
    #include <skyr/core/serialize.hpp>


Example
-------

.. code-block:: c++

    #include <skyr/core/url_record.hpp>
    #include <skyr/core/parse.hpp>
    #include <skyr/core/serialize.hpp>
    #include <print>

    int main() {
      auto result = skyr::parse("http://example.org/path/to/file");

      if (result) {
        const auto& url = result.value();

        std::println("Scheme:   {}", url.scheme);

        if (url.host) {
          std::println("Hostname: {}", url.host->serialize());
        }

        std::print("Pathname: ");
        for (const auto& segment : url.path) {
          std::print("/{}", segment);
        }
        std::println("");

        std::println("Serialized: {}", skyr::serialize(url));
      } else {
        std::println("Parse error: {}", result.error());
      }
    }

API
---

``skyr::host`` class
^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: skyr::host
    :members:

``skyr::url_record`` class
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: skyr::url_record
    :members:

``skyr::url_record`` functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::swap(url_record&, url_record&)

.. doxygenfunction:: skyr::parse(std::string_view)

.. doxygenfunction:: skyr::parse(std::string_view, bool *)

.. doxygenfunction:: skyr::parse(std::string_view, const url_record&)

.. doxygenfunction:: skyr::parse(std::string_view, const url_record&, bool *)

.. doxygenfunction:: skyr::serialize
