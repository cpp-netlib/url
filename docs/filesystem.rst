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

.. code-block:: c++

   #include <skyr/url.hpp>
   #include <skyr/filesystem/path.hpp>
   #include <print>

   int main() {
     auto path = std::filesystem::path("/usr/bin/clang");
     auto url = skyr::filesystem::from_path(path).value();
     std::println(url);
   }

API
---

Path
^^^^

.. doxygenfunction:: skyr::filesystem::from_path

.. doxygenfunction:: skyr::filesystem::to_path

Error codes
^^^^^^^^^^^

.. doxygenenum:: skyr::filesystem::path_errc
