# Skyr URL

## Status

[![License](
    https://img.shields.io/badge/license-boost-blue.svg "License")](
    https://github.com/cpp-netlib/url/blob/master/LICENSE_1_0.txt)
[![Travis Build Status](
    https://travis-ci.org/cpp-netlib/url.svg?branch=master "Build Status")](
    https://travis-ci.org/cpp-netlib/url)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/1iblsi5apka29dmg?svg=true)](
    https://ci.appveyor.com/project/glynos/url-3aeqd)


## Introduction

This library provides:

* A ``skyr::url`` class that implements a generic URL parser,
  conforming with the [WhatWG URL specification](https://url.spec.whatwg.org/#url-class)
* URL serialization and comparison
* Percent encoding and decoding functions
* IDNA and Punycode functions for domain name parsing
* Basic Unicode conversion functions

## Using the library

### ``vcpkg``

``skyr::url`` is available on [``vcpkg``](https://github.com/microsoft/vcpkg):

```bash
> cd ${VCPKG_ROOT}
> git init
> git remote add origin https://github.com/Microsoft/vcpkg.git
> git fetch origin master
> git checkout -b master origin/master
> ./bootstrap-vcpkg.sh
> ./vcpkg install skyr-url
```

On Window, replace the call to ``bootstrap-vcpkg.sh`` with
``bootstrap-vcpkg.bat``.

## Building the project from source

This project requires the availability of a C++17 compliant compiler
and standard library.

### Installing dependencies using `vcpkg`

Using `vcpkg`, install the library dependencies:

```bash
> cd ${VCPKG_ROOT}
> git init
> git remote add origin https://github.com/Microsoft/vcpkg.git
> git fetch origin master
> git checkout -b master origin/master
> ./bootstrap-vcpkg.sh
> ./vcpkg install tl-expected catch2 nlohmann-json fmt
```

### Building with `CMake` and `Ninja`

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

On Windows, replace the target with ``RUN_TESTS``.

To install the library:

```bash
> cmake --build _build --target install
```

## Examples

These examples are based on the
[WhatWG API specification](https://url.spec.whatwg.org/#example-5434421b)

To build the examples with the sources, run `cmake` as follows:

```bash
> cmake .. \
    -G "Ninja" \
    -DSkyr_BUILD_EXAMPLES=ON \
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Creating a URL without a base URL

[This example](examples/example_01.md) parses a string,
"https://example.org/💩", without using a base URL:

```c++
#include <skyr/url.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  auto url = skyr::url("http://example.org/\xf0\x9f\x92\xa9");
  std::cout << url.pathname() << std::endl;
}
```

Gives the output: `/%F0%9F%92%A9`

### Creating a non-absolute URL without a base URL

[This example](examples/example_02.md)  gives an error if
the input, "/🍣🍺", is not an *absolute-URL-with-fragment-string*:

```c++
#include <skyr/url.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  auto url = skyr::make_url("\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
  if (!url) {
    std::cerr << "Parsing failed: " << url.error().message() << std::endl;
  }
}
```

This gives the output: `Parsing failed: Not an absolute URL with fragment`

### Creating a non-absolute URL with a base URL

[This example](examples/example_03.md) parses a string,
"🏳️‍🌈", using a base URL, "https://example.org/":

```c++
#include <skyr/url.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  auto base = skyr::url("https://example.org/");
  auto url = skyr::url(
    "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", base);
  std::cout << url.href() << std::endl;
}
```

This gives the output: `https://example.org/%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88`

## Installation

### Installing with `CMake` and `Ninja`

```bash
> cmake .. \
    -G "Ninja" \
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_INSTALL_PREFIX=$PREFIX
> ninja
> ninja test      # optional
> ninja install
```

Where `$PREFIX` is the location where you want to install the
library. Depending on the location of `$PREFIX`, you may need to run
the install command as an administrator (e.g. on Linux as `sudo`).

### Installing with `CMake` and `Visual Studio 2017`

Open the `CMake` menu, click `Install`, then `Skyr`.

## Dependencies

This library uses [expected](https://github.com/TartanLlama/expected)
and a modified implementation of [utfcpp](https://github.com/nemtrif/utfcpp).

The tests use [Catch2](https://github.com/catchorg/catch2),
[nlohmann-json](https://github.com/nlohmann/json) and
[fmtlib](https://github.com/fmtlib/fmt).

## Requirements

This library has been tested using the following platforms and
compilers:

Linux:

* GCC 8
* GCC 9
* Clang 7
* Clang 8
* Clang 9

MacOS:

* Clang 7
* Clang 8
* Clang 9

Windows:

* Microsoft Visual C++ 2017
* Microsoft Visual C++ 2019

## License

This library is released under the Boost Software License (please see
http://boost.org/LICENSE_1_0.txt or the accompanying LICENSE_1_0.txt
file for the full text).

## Why *skyr*?

This name was chosen by a random project name generator, which
itself was randomly chosen.

## Contact

Any questions about this library can be addressed to the cpp-netlib
[developers mailing list]. Issues can be filed using Github at
http://github.com/cpp-netlib/url/issues.

[developers mailing list]: cpp-netlib@googlegroups.com
