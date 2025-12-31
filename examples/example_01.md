# Example 1 - Creating a URL without a base URL

## Source

The example is equivalent to one described in the
[WhatWG URL specification](https://url.spec.whatwg.org/#example-5434421b).

```c++
#include <iostream>
#include <skyr/url.hpp>

int main() {
  auto url = skyr::url("http://example.org/\xf0\x9f\x92\xa9");
  std::cout << url << std::endl;
  std::cout << url.pathname() << std::endl;
}
```

The ``skyr::url`` works like this: it's constructor parses
the input string. Parser errors are handled by throwing a
``url_parse_error`` exception.

Once the ``url`` object is constructed, the parts can be
access through accessors, such as ``pathname``.

## Configuration

To build this example in a project using
[``vcpkg``](https://github.com/microsoft/vcpkg) and CMake,
use the following set up:

```cmake
cmake_minimum_required(VERSION 3.14)
project(example_01)

set(CMAKE_CXX_STANDARD 17)

find_package(tl-expected CONFIG REQUIRED)
find_package(skyr-url CONFIG REQUIRED)

add_executable(example_01 example_01.cpp)
target_link_libraries(example_01 PRIVATE skyr::url)
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
