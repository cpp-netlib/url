.. Skyr documentation master file, created by
   sphinx-quickstart on Sat Sep 22 17:34:21 2018.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Skyr URL
========

Introduction
------------

This library provides:

- A `skyr::url` class that implements a generic URL parser,
  conforming with the WhatWG URL specification
- URL serialization and comparison
- **Immutable URL transformations** with `with_*` methods for functional-style URL building
- **URL sanitization** methods to remove credentials, fragments, and query parameters
- **Custom `std::format` support** with format specifiers for URL components
- Percent encoding and decoding functions
- IDNA and Punycode functions for domain name parsing
- Unicode conversion utilities

Quick Start
-----------

This project requires a **C++23 compliant compiler** (GCC 13+, Clang 19+, MSVC 2022+)
and has **zero external dependencies** for core URL parsing.

1. Download ``vcpkg`` and install the dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   > cd ${VCPKG_ROOT}
   > git init
   > git remote add origin https://github.com/Microsoft/vcpkg.git
   > git fetch origin master
   > git checkout -b master origin/master
   > ./bootstrap-vcpkg.sh
   > ./vcpkg install skyr-url

2. Write your code
^^^^^^^^^^^^^^^^^^

.. code-block:: c++

   // url_test.cpp

   #include <skyr/url.hpp>
   #include <skyr/url_format.hpp>
   #include <print>

   int main() {
     auto url = skyr::url("http://sub.example.إختبار:8090/\xcf\x80?a=1&c=2&b=\xe2\x80\x8d\xf0\x9f\x8c\x88");

     // Using std::format support
     std::println("Scheme:   {:s}", url);
     std::println("Domain?   {}", url.is_domain());
     std::println("Domain:   {:h}", url);    // Encoded (punycode)
     std::println("Domain:   {:hd}", url);   // Decoded (unicode)
     std::println("Port:     {:p}", url);
     std::println("Pathname: {:Pd}", url);   // Decoded pathname

     std::println("\nSearch parameters:");
     const auto &search = url.search_parameters();
     for (const auto &[key, value] : search) {
       std::println("  key: {}, value = {}", key, value);
     }
   }

3. Set up your build dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   # CMakeLists.txt

   cmake_minimum_required(VERSION 3.21)
   project(my_project)

   find_package(skyr-url CONFIG REQUIRED)

   set(CMAKE_CXX_STANDARD 23)

   add_executable(url_test url_test.cpp)
   target_link_libraries(url_test PRIVATE skyr::skyr-url)

4. Build and run
^^^^^^^^^^^^^^^^

.. code-block:: bash

   > cd ${SOURCE_ROOT}
   > cmake \
         -B _build \
         -G Ninja \
         -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/vcpkg/scripts/buildsystems/vcpkg.cmake \
         .
   > cmake --build _build
   > ./_build/url_test

Design objectives
^^^^^^^^^^^^^^^^^

* Uses modern C++23 features (``std::expected``, ``std::format``, ``std::ranges``)
* Header-only library with zero external dependencies
* Cross-platform (Linux, macOS, Windows)
* Easy to use and read with immutable, functional-style API
* Compliant with the WhatWG URL specification
* Works naturally with Unicode strings (IDNA/Punycode support)
* Uses modern CMake and is available via vcpkg

Documentation
-------------

.. toctree::
   :maxdepth: 2

   url
   core
   domain
   network
   percent_encoding
   filesystem
   json
   changelog