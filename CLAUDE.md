# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**skyr-url** is a C++ library that implements a generic URL parser conforming to the WhatWG URL specification. The library provides:

- A `skyr::url` class for URL parsing, serialization, and comparison
- Percent encoding and decoding functions
- IDNA and Punycode functions for domain name parsing
- Unicode conversion utilities

## Multi-Version Architecture

The codebase maintains **two parallel versions** (v1, v2) to support different C++ standards:

- **v1**: C++17 compatible (legacy, `skyr_BUILD_V1=OFF` by default)
- **v2**: C++20 features (`skyr_BUILD_V2=ON` by default, inlined namespace)

Each version lives in separate namespaces (`skyr::v1`, `skyr::inline v2`) with corresponding directory structures under `include/skyr/v{1,2}/` and `src/v{1,2}/`. The v2 namespace is inlined by default, so `skyr::url` resolves to `skyr::v2::url`.

When making changes, consider which version(s) require updates. The architecture allows multiple versions to coexist without conflicts.

**Note**: A `skyr_BUILD_V3` option exists (OFF by default) for future C++23 development, but v3 is not currently implemented.

## Building

### Dependencies

Install via vcpkg:
```bash
cd ${VCPKG_ROOT}
./vcpkg install tl-expected range-v3 catch2 nlohmann-json fmt
```

### Configure and Build

```bash
mkdir _build
cmake \
  -B _build \
  -G "Ninja" \
  -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
  .
cmake --build _build
```

### CMake Options

Key build options (see CMakeLists.txt:23-36):
- `skyr_BUILD_TESTS` (ON): Build tests
- `skyr_BUILD_V1` (OFF): Build C++17 version
- `skyr_BUILD_V2` (ON): Build C++20 version (default)
- `skyr_ENABLE_FILESYSTEM_FUNCTIONS` (ON): Enable filesystem::path conversion
- `skyr_ENABLE_JSON_FUNCTIONS` (ON): Enable JSON serialization
- `skyr_BUILD_WITHOUT_EXCEPTIONS` (OFF): Build without exceptions

## Testing

### Run All Tests

```bash
cmake --build _build --target test
```

On Windows:
```bash
cmake --build _build --target RUN_TESTS
```

### Test Organization

Tests are organized by version under `tests/v{1,2}/`, then by component:
- `containers/` - Container data structure tests
- `unicode/` - Unicode conversion tests
- `domain/` - IDNA and Punycode tests
- `percent_encoding/` - Percent encoding/decoding tests
- `network/` - IPv4/IPv6 address tests
- `core/` - URL parsing core tests
- `url/` - Main URL class tests
- `filesystem/` - Filesystem path conversion tests (if enabled)
- `json/` - JSON serialization tests (if enabled)

### Running Individual Tests

The `skyr_create_test()` function in `tests/CMakeLists.txt:1-23` creates test executables with the pattern `{test_basename}-{version}` in `_build/tests/{component}/`.

Example for v2 URL tests:
```bash
# Run specific test executable
./_build/tests/url/url_tests-v2

# Use CTest to run specific test
ctest --test-dir _build -R url_tests-v2
```

### Adding New Tests

1. Create `.cpp` file in appropriate `tests/v{version}/{component}/` directory
2. Add to the component's `CMakeLists.txt` using the `foreach` pattern:
   ```cmake
   foreach (file_name
           your_new_test.cpp
           )
       skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/{component} test_name v2)
   endforeach ()
   ```

## Code Structure

### Core Components

Each version implements these components independently:

- **core/**: URL parsing state machine, serialization
  - `parse.hpp`: URL parsing according to WhatWG algorithm
  - `serialize.hpp`: URL serialization
  - `url_record.hpp`: Internal URL representation
  - `schemes.hpp`: Special scheme handling
  - `errors.hpp`: Error codes

- **domain/**: Domain name processing
  - `domain.hpp`: Domain validation and processing
  - `idna.hpp`: Internationalized Domain Names in Applications
  - `punycode.hpp`: Punycode encoding/decoding
  - `idna_tables.hpp` (v2+): Unicode IDNA tables

- **percent_encoding/**: Percent encoding utilities
  - `percent_encode.hpp`: Encoding functions
  - `percent_decode.hpp`: Decoding functions

- **network/**: IP address parsing
  - `ipv4_address.hpp`: IPv4 address parsing
  - `ipv6_address.hpp`: IPv6 address parsing

- **unicode/**: Unicode conversion utilities
  - `core.hpp`: Core conversion functions
  - `code_point.hpp`: Code point utilities
  - `ranges/`: Range-based views for UTF transformations

### Public API

The main user-facing class is `skyr::url` (defined in `include/skyr/v{1,2}/url.hpp`). The top-level `include/skyr/url.hpp` forwards to v1 for backward compatibility.

## Library Targets

The library creates interface targets for each version:
- `skyr-url-v{1,2}`: Core URL library
- `skyr-filesystem-v{1,2}`: Filesystem extensions (optional)
- `skyr-json-v{1,2}`: JSON extensions (optional)

When used as a dependency, v1 provides aliases (only when v1 is enabled):
- `skyr::skyr-url` / `skyr::url`
- `skyr::skyr-filesystem` / `skyr::filesystem`
- `skyr::skyr-json` / `skyr::json`

## Key Dependencies

- **tl-expected**: Error handling via `tl::expected<T, E>`
- **range-v3**: Range-based algorithms and views
- **nlohmann-json** (optional): JSON serialization
- **Catch2** (tests): Testing framework
- **fmt** (tests): String formatting