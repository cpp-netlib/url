Domain name processing
======================

IDNA
----

.. doxygenenum:: skyr::v1::unicode::idna_status

.. doxygenfunction:: skyr::v1::unicode::map_idna_status

.. doxygenfunction:: skyr::v1::unicode::map_idna_code_point

Error codes
-----------

.. doxygenenum:: skyr::v1::unicode::domain_errc

Punycode
--------

.. doxygenfunction:: skyr::v1::unicode::punycode_encode(std::string_view)

.. doxygenfunction:: skyr::v1::unicode::punycode_decode(std::string_view)

Domain to ASCII
---------------

.. doxygenfunction:: skyr::v1::unicode::domain_to_ascii(std::string_view, bool)
