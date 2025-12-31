# Example 2 - Creating a non-absolute URL without a base URL

## Source

The example is equivalent to one described in the
[WhatWG URL specification](https://url.spec.whatwg.org/#example-5434421b).

```c++
#include <iostream>
#include <skyr/url.hpp>

int main() {
  auto url = skyr::make_url("\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
  if (!url) {
    std::cerr << "Parsing failed: " << url.error().message() << std::endl;
  }
}
```

``skyr::make_url`` parses the input string and constructs
a ``skyr::url`` object. However, instead of throwing an
exception when the parser fails, this function returns a
``tl::expected`` which contains a ``std::error_code``.

## Configuration

To build this example in a project using
[``vcpkg``](https://github.com/microsoft/vcpkg) and CMake,
use the following set up:

```cmake
cmake_minimum_required(VERSION 3.14)
project(example_02)

set(CMAKE_CXX_STANDARD 17)

find_package(tl-expected CONFIG REQUIRED)
find_package(skyr-url CONFIG REQUIRED)

add_executable(example_02 example_02.cpp)
target_link_libraries(example_02 PRIVATE skyr::url)
```

and to build it, run cmake like this:

```bash
> mkdir _build
> cd _build
> cmake \
  -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/vcpkg/scripts/buildsystems/vcpkg.cmake \
  ..
> ninja
```
