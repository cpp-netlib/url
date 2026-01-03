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
   #include <filesystem>

   int main() {
     // Convert filesystem path to URL
     auto path = std::filesystem::path("/usr/bin/clang");
     auto url = skyr::filesystem::from_path(path);
     if (url) {
       std::println("URL: {}", url->href());
       // Output: file:///usr/bin/clang
     }

     // Convert URL back to filesystem path
     auto file_url = skyr::url("file:///home/user/documents/report.pdf");
     auto fs_path = skyr::filesystem::to_path(file_url);
     if (fs_path) {
       std::println("Path: {}", fs_path->string());
       // Output: /home/user/documents/report.pdf
     }

     // Works with Unicode paths
     auto unicode_path = std::filesystem::path("/tmp/тест.txt");
     auto unicode_url = skyr::filesystem::from_path(unicode_path);
     if (unicode_url) {
       std::println("Encoded URL: {}", unicode_url->href());
       // Output: file:///tmp/%D1%82%D0%B5%D1%81%D1%82.txt
     }
   }

API
---

Path Functions
^^^^^^^^^^^^^^

.. doxygenfunction:: skyr::filesystem::from_path

.. doxygenfunction:: skyr::filesystem::to_path

Error Codes
^^^^^^^^^^^

.. doxygenenum:: skyr::filesystem::path_errc
