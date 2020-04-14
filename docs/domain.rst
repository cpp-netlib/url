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

    #include <skyr/domain/punycode.hpp>
    #include <skyr/domain/domain.hpp>

Example
-------

API
---

Punycode
^^^^^^^^

.. doxygenfunction:: skyr::punycode_encode(std::string_view)

.. doxygenfunction:: skyr::punycode_decode(std::string_view)

Domain to ASCII
^^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::domain_to_ascii(std::string_view, bool)

.. doxygenfunction:: skyr::domain_to_unicode(std::string_view, bool)

Error codes
^^^^^^^^^^^

.. doxygenenum:: skyr::v1::domain_errc
