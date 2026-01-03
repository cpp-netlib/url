URL
===

Description
-----------

The ``skyr::url`` class parses a URL in the constructor and provides
a rich interface to access and process the URL components. The library
uses C++23 ``std::expected`` for error handling, providing type-safe
error propagation without exceptions.

Key features:

* Immutable URL transformations with ``with_*`` methods
* URL sanitization methods to remove credentials, fragments, and query parameters
* Custom ``std::format`` support with format specifiers for URL components
* Percent encoding and decoding
* IDNA and Punycode for internationalized domain names

Headers
-------

.. code-block:: c++

    #include <skyr/url.hpp>

Basic Example
-------------

.. code-block:: c++

    #include <skyr/url.hpp>
    #include <print>

    int main() {
      auto url = skyr::url("http://example.com/path?query=1");

      std::println("Scheme:   {}", url.scheme());
      std::println("Hostname: {}", url.hostname());
      std::println("Pathname: {}", url.pathname());
      std::println("Search:   {}", url.search());
    }

Immutable Transformations
--------------------------

.. code-block:: c++

    #include <skyr/url.hpp>
    #include <print>

    int main() {
      auto dev_url = skyr::url("http://localhost:3000/api/v1/users");

      // Transform development URL to production
      auto prod_url = dev_url.with_scheme("https")
                          .and_then([](auto&& u) { return u.with_hostname("api.example.com"); })
                          .and_then([](auto&& u) { return u.with_port(""); })
                          .and_then([](auto&& u) { return u.with_pathname("/api/v2/users"); });

      if (prod_url) {
        std::println("Production: {}", prod_url->href());
      }
    }

URL Sanitization
----------------

.. code-block:: c++

    #include <skyr/url.hpp>
    #include <print>

    int main() {
      auto url = skyr::url("http://user:pass@example.com/path?debug=true&id=123#section");

      // Remove credentials and fragment
      auto safe_url = url.sanitize();
      std::println("{}", safe_url.href());
      // Output: http://example.com/path?debug=true&id=123

      // Chain operations to remove specific parameters
      auto clean_url = url.sanitize().without_params({"debug"});
      std::println("{}", clean_url.href());
      // Output: http://example.com/path?id=123
    }

API
---

``skyr::url`` class
^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: skyr::url
    :members:

``skyr::url`` functions
^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::swap(url&, url&)

.. doxygenfunction:: skyr::make_url(const Source&)

.. doxygenfunction:: skyr::make_url(const Source&, const url&)

``skyr::url`` error codes
^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenenum:: skyr::url_parse_errc

.. doxygenclass:: skyr::url_parse_error
    :members:

``skyr::url_search_parameters`` class
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: skyr::url_search_parameters
    :members:
