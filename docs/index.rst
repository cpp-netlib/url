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
- Percent encoding and decoding functions
- IDNA and Punycode functions for domain name parsing
- Basic Unicode conversion functions

Quick Start
-----------

This project requires the availability of a C++17 compliant compiler
and standard library.

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
   #include <skyr/percent_encoding/percent_decode.hpp>
   #include <iostream>

   int main() {
     using namespace skyr::literals;

     try {
       auto url =
           "http://sub.example.إختبار:8090/\xcf\x80?a=1&c=2&b=\xe2\x80\x8d\xf0\x9f\x8c\x88"_url;

       std::cout << "Protocol: " << url.protocol() << std::endl;

       std::cout << "Domain?   " << std::boolalpha << url.is_domain() << std::endl;
       std::cout << "Domain:   " << url.hostname() << std::endl;
       std::cout << "Domain:   "
                 << skyr::domain_to_u8(url.hostname()).value() << std::endl;

       std::cout << "Port:     " << url.port<std::uint16_t>().value() << std::endl;

       std::cout << "Pathname: "
                 << skyr::percent_decode<std::string>(url.pathname()).value() << std::endl;

       std::cout << "Search parameters" << std::endl;
       const auto &search = url.search_parameters();
       for (const auto &[key, value] : search) {
         std::cout << "  " << "key: " << key << ", value = " << value << std::endl;
       }
     }
     catch (const skyr::url_parse_error &e) {
       std::cout << e.code().message() << std::endl;
     }
   }

3. Set up your build dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   # CMakeLists.txt

   project(my_project)

   find_package(tl-expected CONFIG REQUIRED)
   find_package(skyr-url CONFIG REQUIRED)

   set(CMAKE_CXX_STANDARD 17)

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

* Uses modern C++17 features
* Cross-platform
* Easy to use and read
* Compliant to the the WhatWG URL standard
* Works naturally with Unicode strings
* Uses modern CMake and is available as a dependency using a package
  manager

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