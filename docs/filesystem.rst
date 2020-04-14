Filesystem Path Conversion
==========================

Description
-----------

The functions described below are convenience functions to
convert between a URL and a filesystem path.

Headers
-------

.. code-block:: c++

    #include <skyr/filesystem/path.hpp>

Example
-------

API
---

Path
^^^^

.. doxygenfunction:: skyr::v1::filesystem::from_path

.. doxygenfunction:: skyr::v1::filesystem::to_path

Error codes
^^^^^^^^^^^

.. doxygenenum:: skyr::v1::filesystem::path_errc
