# Skyr URL

## Status

[![License](
    https://img.shields.io/badge/license-boost-blue.svg "License")](
    https://github.com/cpp-netlib/url/blob/master/LICENSE_1_0.txt)
[![GitHub Actions Status](
    https://github.com/cpp-netlib/url/workflows/skyr-url%20CI/badge.svg?branch=main)](
    https://github.com/cpp-netlib/url/actions?query=workflow%3A%22skyr-url+CI%22)
    
## Notice

I've changed the name of the default branch from `master` to `main`. Please make future
PRs to the `main` branch on the [cpp-netlib repo](https://github.com/cpp-netlib/url).

## Introduction

This library provides:

* A ``skyr::url`` class that implements a generic URL parser,
  conforming with the [WhatWG URL specification](https://url.spec.whatwg.org/#url-class)
* URL serialization and comparison
* Percent encoding and decoding functions
* IDNA and Punycode functions for domain name parsing
* Basic Unicode conversion functions

## Using the library

This project requires the availability of a C++17 compliant compiler
and standard library.

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

Using `vcpkg`, install the library dependencies:

```bash
> cd ${VCPKG_ROOT}
> git init
> git remote add origin https://github.com/Microsoft/vcpkg.git
> git fetch origin master
> git checkout -b master origin/master
> ./bootstrap-vcpkg.sh
> ./vcpkg install tl-expected range-v3 catch2 nlohmann-json fmt
```

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

To install the library:

```bash
> cmake --build _build --target install
```

## Testing and installing the project

### Installing with `CMake` and `Ninja`

```bash
> cmake .. \
    -G "Ninja" \
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_INSTALL_PREFIX=$PREFIX
> ninja
> ninja test
> ninja install
```

Where `$PREFIX` is the location where you want to install the
library. Depending on the location of `$PREFIX`, you may need to run
the install command as an administrator (e.g. on Linux as `sudo`).


## Example usage

### Source code

Here is an example of how to use the ``skyr::url`` class to parse a
URL string and to process the components:

```c++
// url_parts.cpp

#include <skyr/url.hpp>
#include <skyr/percent_encoding/percent_decode.hpp>
#include <iostream>

int main() {
  using namespace skyr::literals;

  auto url =
      "http://sub.example.إختبار:8090/\xcf\x80?a=1&c=2&b=\xe2\x80\x8d\xf0\x9f\x8c\x88"_url;

  std::cout << "Protocol: " << url.protocol() << std::endl;

  std::cout << "Domain?   " << std::boolalpha << url.is_domain() << std::endl;
  std::cout << "Domain:   " << url.hostname() << std::endl;
  std::cout << "Domain:   " << url.u8domain().value() << std::endl;

  std::cout << "Port:     " << url.port<std::uint16_t>().value() << std::endl;

  std::cout << "Pathname: "
            << skyr::percent_decode(url.pathname()).value() << std::endl;

  std::cout << "Search parameters:" << std::endl;
  const auto &search = url.search_parameters();
  for (const auto &[key, value] : search) {
    std::cout << "  " << "key: " << key << ", value = " << value << std::endl;
  }
}
```

### Build script

Here is the ``CMake`` script to build the example:

```cmake
# CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

project(my_project)

find_package(tl-expected CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(skyr-url CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(url_parts url_parts.cpp)
target_link_libraries(url_parts PRIVATE skyr::skyr-url)
```

### Output

The output of this program is:

```bash
Protocol: http:
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

This library uses [expected](https://github.com/TartanLlama/expected)
and [Range v3](https://github.com/ericniebler/range-v3).

The tests use [Catch2](https://github.com/catchorg/catch2),
[nlohmann-json](https://github.com/nlohmann/json) and
[fmtlib](https://github.com/fmtlib/fmt).

## Acknowledgements

This library includes a modified implementation of [utfcpp](https://github.com/nemtrif/utfcpp).

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
