Core
====

Description
-----------

The core of the ``skyr`` library is a ``skyr::parse`` function
that parses an input string and creates a ``skyr::url_record``
that contains a URL parts, and a ``skyr::serialize`` function
that composes a URL string from an existing ``skyr::url_record``.

Use these functions directly if your needs are simple. Use the
``skyr::url`` class for more complex operations, including
Unicode encoding and decoding.

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
    #include <iostream>

    int main() {
      auto url = skyr::url_parse("http://example.org/");
      std::cout << url << std::endl;
      std::cout << "Scheme:     " << url.scheme << std::endl;
      std::cout << "Hostname:   " << url.host.value().to_string());
      std::cout << "Pathname:   ";
      for (const auto &path : url.path) {
        << std::cout << "/" << path;
      }
      std::cout << std::endl;

      std::cout << "Serializer: " << skyr::serialize(url) << std::endl;
    }

API
---

``skyr::host`` class
^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: skyr::v1::host
    :members:

``skyr::url_record`` class
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: skyr::v1::url_record
    :members:

``skyr::url_record`` functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::swap(url_record&, url_record&)

.. doxygenfunction:: skyr::parse(std::string_view)

.. doxygenfunction:: skyr::parse(std::string_view, bool *)

.. doxygenfunction:: skyr::parse(std::string_view, const url_record&)

.. doxygenfunction:: skyr::parse(std::string_view, const url_record&, bool *)

.. doxygenfunction:: skyr::serialize
