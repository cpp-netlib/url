URL
===

Description
-----------

The ``skyr::url`` class parses a URL in the constructor and provides
a rich interface to access and process the URL components. The
``skyr::url`` constructor throws a ``skyr::url_parse_error`` on failure.

Headers
-------

.. code-block:: c++

    #include <skyr/url.hpp>

Example
-------

.. code-block:: c++

    #include <skyr/url.hpp>
    #include <iostream>

    int main() {
      using namespace skyr::literals;

      try {
        auto url = "http://example.com/"_url;

        std::cout << "Protocol: " << url.protocol() << std::endl;
        std::cout << "Hostname: " << url.hostname() << std::endl;
        std::cout << "Pathname: " << url.pathname() << std::endl;
      }
      catch (const skyr::url_parse_error &e) {
        std::cout << e.code().message() << std::endl;
      }
    }

API
---

``skyr::url`` class
^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: skyr::v1::url
    :members:

``skyr::url`` functions
^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::swap(url&, url&)

.. doxygenfunction:: skyr::make_url(const Source&)

.. doxygenfunction:: skyr::make_url(const Source&, const url&)

``skyr::url`` error codes
^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenenum:: skyr::v1::url_parse_errc

.. doxygenclass:: skyr::v1::url_parse_error
    :members:

``skyr::url_search_parameters`` class
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: skyr::v1::url_search_parameters
    :members:
