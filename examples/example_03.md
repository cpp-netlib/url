# Example 3 - Creating a non-absolute URL with a base URL

## Source

The example is equivalent to one described in the
[WhatWG URL specification](https://url.spec.whatwg.org/#example-5434421b).

```c++
#include <iostream>
#include <skyr/url.hpp>

int main() {
  auto base = skyr::url("https://example.org/");
  auto url = skyr::url(
      "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", base);
  std::cout << url << std::endl;
}
```
This example parses a string, "🏳️‍🌈", using a base URL,
"https://example.org/".

## Configuration

To build this example in a project using
[``vcpkg``](https://github.com/microsoft/vcpkg) and CMake,
use the following set up:

```cmake
cmake_minimum_required(VERSION 3.14)
project(example_03)

set(CMAKE_CXX_STANDARD 17)

find_package(tl-expected CONFIG REQUIRED)
find_package(skyr-url CONFIG REQUIRED)

add_executable(example_03 example_03.cpp)
target_link_libraries(example_03 PRIVATE skyr::url)
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
