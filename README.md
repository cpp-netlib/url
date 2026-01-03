# Skyr URL

## Status

[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://github.com/cpp-netlib/url/blob/master/LICENSE_1_0.txt)
[![CI](https://github.com/cpp-netlib/url/actions/workflows/skyr-url-ci.yml/badge.svg)](https://github.com/cpp-netlib/url/actions/workflows/skyr-url-ci.yml)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Version](https://img.shields.io/badge/version-3.0.0-blue.svg)](https://github.com/cpp-netlib/url/releases)
[![vcpkg](https://img.shields.io/badge/vcpkg-available-brightgreen.svg)](https://vcpkg.io/en/package/skyr-url)

[![Header-Only](https://img.shields.io/badge/header--only-yes-success.svg)](https://github.com/cpp-netlib/url/tree/main/include)
[![Dependencies](https://img.shields.io/badge/dependencies-zero-success.svg)](https://github.com/cpp-netlib/url#dependencies)
[![WhatWG URL](https://img.shields.io/badge/spec-WhatWG%20URL-orange.svg)](https://url.spec.whatwg.org/)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)](https://github.com/cpp-netlib/url#platform-support)

## Introduction

This library provides:

* A ``skyr::url`` class that implements a generic URL parser,
  conforming with the [WhatWG URL specification](https://url.spec.whatwg.org/#url-class)
* URL serialization and comparison
* **Immutable URL transformations** with `with_*` methods for functional-style URL building
* **URL sanitization** methods to remove credentials, fragments, and query parameters
* **`std::format` support** with custom format specifiers for URL components
* Percent encoding and decoding functions
* IDNA and Punycode functions for domain name parsing
* Unicode conversion utilities

## Modern C++23 Implementation

**This library has been modernized to use C++23 exclusively**, leveraging the latest standard library features:

* **`std::expected`** for error handling (no external dependency needed!)
* **`std::format`** for string formatting
* **`std::ranges`** for functional-style operations
* **Zero external dependencies** - Unicode/IDNA/Punycode support built-in

## Using the library

This project requires:
* A **C++23 compliant compiler** (GCC 13+, Clang 19+, MSVC 2022+)
* **No external dependencies** for core URL parsing

### ``vcpkg``

``skyr::url`` is available on [``vcpkg``](https://github.com/microsoft/vcpkg).
It can be installed by executing the following steps:

```bash
> cd ${VCPKG_ROOT}
> git init
> git remote add origin https://github.com/Microsoft/vcpkg.git
> git fetch origin master
> git checkout -b master origin/master
> ./bootstrap-vcpkg.sh
> ./vcpkg install skyr-url
```

On Windows - for example, using Powershell - replace the
call to ``bootstrap-vcpkg.sh`` with ``bootstrap-vcpkg.bat``.

## Building the project from source

### Installing dependencies using `vcpkg`

Using `vcpkg`, install the optional test dependencies:

```bash
> cd ${VCPKG_ROOT}
> git init
> git remote add origin https://github.com/Microsoft/vcpkg.git
> git fetch origin master
> git checkout -b master origin/master
> ./bootstrap-vcpkg.sh
> ./vcpkg install catch2 nlohmann-json
```

**Note**: The library has zero dependencies. `catch2` and `nlohmann-json` are only needed for tests and optional JSON functionality.

### Building the project with `CMake` and `Ninja`

From a terminal, execute the following sequence of commands:

```bash
> mkdir _build
> cmake \
    -B _build \
    -G "Ninja" \
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/vcpkg/scripts/buildsystems/vcpkg.cmake \
    .
> cmake --build _build
```

To run the tests:

```bash
> cmake --build _build --target test
```

On Windows, replace the target with ``RUN_TESTS``:

```powershell
> cmake --build _build --target RUN_TESTS
```

To install the library (optional):

```bash
> cmake --build _build --target install
```

Or with a custom install prefix:

```bash
> cmake \
    -B _build \
    -G "Ninja" \
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_INSTALL_PREFIX=/your/install/path \
    .
> cmake --build _build --target install
```

**Note**: Depending on the install location, you may need administrator privileges (e.g., `sudo` on Linux).

## Example usage

### Source code

Here is an example of how to use the ``skyr::url`` class to parse a
URL string and to process the components:

```c++
// url_parts.cpp

#include <print>

#include <skyr/url.hpp>
#include <skyr/url_format.hpp>

int main() {
  using namespace skyr::literals;

  auto url =
      "http://sub.example.إختبار:8090/\xcf\x80?a=1&c=2&b=\xe2\x80\x8d\xf0\x9f\x8c\x88"_url;


  std::println("Origin: {:o}", url);
  std::println("Protocol: {:s}", url);
  std::println("Domain?   {}", url.is_domain());
  std::println("Domain:   {:h}", url);   // Encoded (punycode)
  std::println("Domain:   {:hd}", url);  // Decoded (unicode)
  std::println("Port:     {:p}", url);
  std::println("Pathname: {:Pd}", url);  // Decoded pathname

  std::println("Search parameters:");
  const auto &search = url.search_parameters();
  for (const auto &[key, value] : search) {
    std::println("  key: {}, value = {}", key, value);
  }
}
```

### Build script

Here is the ``CMake`` script to build the example:

```cmake
# CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

project(my_project)

find_package(skyr-url CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(url_parts url_parts.cpp)
target_link_libraries(url_parts PRIVATE skyr::skyr-url)
```

### Output

The output of this program is:

```bash
Origin: http://sub.example.xn--kgbechtv:8090
Protocol: http
Domain?   true
Domain:   sub.example.xn--kgbechtv
Domain:   sub.example.إختبار
Port:     8090
Pathname: /π
Search parameters:
  key: a, value = 1
  key: c, value = 2
  key: b, value = ‍🌈
```

## Dependencies

This library leverages **C++23 standard library features**:
* **`std::expected`** (C++23) - Error handling
* **`std::format`** (C++23) - String formatting
* **`std::ranges`** (C++20/23) - Range algorithms and views

**Core library**: **Zero external dependencies!**
* Unicode/IDNA/Punycode support via custom header-only implementation

**Test dependencies** (optional):
* [Catch2](https://github.com/catchorg/catch2) - Testing framework
* [nlohmann-json](https://github.com/nlohmann/json) - JSON support (for optional JSON features)

## Platform support

Look at the [GitHub Actions Status](https://github.com/cpp-netlib/url/actions)
for all of the configurations for which this library is tested.

## License

This library is released under the Boost Software License (please see
http://boost.org/LICENSE_1_0.txt or the accompanying [LICENSE_1_0.txt](LICENSE_1_0.txt)
file for the full text).

## Why *skyr*?

This name was chosen by a random project name generator, which
itself was randomly chosen.

## Contact

Any questions about this library can be addressed to the cpp-netlib
[developers mailing list](cpp-netlib@googlegroups.com). Issues can
be filed on our [GitHub page](http://github.com/cpp-netlib/url/issues).

You can also contact me via Twitter [@glynos](https://twitter.com/glynos).
