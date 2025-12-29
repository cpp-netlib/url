# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**skyr-url** is a modern C++23 library that implements a generic URL parser conforming to the WhatWG URL specification. The library provides:

- A `skyr::url` class for URL parsing, serialization, and comparison
- Percent encoding and decoding functions
- IDNA and Punycode functions for domain name parsing
- Unicode conversion utilities

## Architecture

**Header-Only Library**: This is a pure header-only library - all implementation is in `include/skyr/`. No compilation required!

**C++23-Only Implementation**: As of the latest reboot, this library is C++23-only. Previous v1 (C++17) and v2 (C++20) versions have been removed to focus on modern C++ features.

**Modern C++ Features Used**:
- `std::expected<T, E>` for error handling (replaces `tl::expected`)
- `std::format` for string formatting (replaces `fmt::format`)
- `std::ranges` for range-based algorithms and views (replaces `range-v3`)
- `uni-algo` library for Unicode processing

**Key Advantages**:
- **Header-only** - just include and use, no linking required
- **Minimal external dependencies** - only requires `uni-algo` for Unicode support

## Building

### Dependencies

**Required**:
- C++23-compliant compiler (GCC 13+, Clang 16+, MSVC 2022 17.6+)

**Optional** (automatically disabled with warnings if not found):
- `uni-algo` for full Unicode/IDNA processing
- `catch2` for tests
- `nlohmann-json` for JSON functionality

To install optional dependencies:
```bash
cd ${VCPKG_ROOT}
./vcpkg install uni-algo catch2 nlohmann-json
```

**Note**: The library will work for basic URL parsing even without dependencies, but IDNA/Punycode (internationalized domain names) require `uni-algo`.

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

Key build options:
- `skyr_BUILD_TESTS` (ON): Build tests
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

Tests are organized under `tests/` by component:
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

```bash
# Run specific test executable
./_build/tests/url/url_tests

# Use CTest to run specific test
ctest --test-dir _build -R url_tests

# Run all tests
ctest --test-dir _build
```

### Adding New Tests

1. Create `.cpp` file in appropriate `tests/{component}/` directory
2. Add to the component's `CMakeLists.txt` using the `foreach` pattern:
   ```cmake
   foreach (file_name
           your_new_test.cpp
           )
       skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/{component} test_name)
   endforeach ()
   ```

## Code Structure

**Directory Layout**:
- `include/skyr/` - All header files (the actual implementation)
- `cmake/targets/` - CMake target definitions (no source code, just build configuration)
- `tests/` - Comprehensive test suite

### Core Components

All components are in the `skyr` namespace under `include/skyr/`:

- **core/**: URL parsing state machine, serialization
  - `parse.hpp`: URL parsing according to WhatWG algorithm
  - `serialize.hpp`: URL serialization
  - `url_record.hpp`: Internal URL representation
  - `schemes.hpp`: Special scheme handling
  - `errors.hpp`: Error codes
  - `host.hpp`: Host parsing (domain, IPv4, IPv6, opaque)

- **domain/**: Domain name processing
  - `domain.hpp`: Domain validation and IDNA processing
  - `idna.hpp`: Internationalized Domain Names in Applications
  - `punycode.hpp`: Punycode encoding/decoding
  - `idna_table.hpp`: Unicode IDNA tables

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

The main user-facing class is `skyr::url` (defined in `include/skyr/url.hpp`).

## Library Targets

The library creates interface targets:
- `skyr-url`: Core URL library
- `skyr-filesystem`: Filesystem extensions (optional)
- `skyr-json`: JSON extensions (optional)

Aliases for compatibility:
- `skyr::skyr-url` / `skyr::url`
- `skyr::skyr-filesystem` / `skyr::filesystem`
- `skyr::skyr-json` / `skyr::json`

## Key Dependencies

- **C++23 standard library**: `std::expected`, `std::format`, `std::ranges`
- **uni-algo**: Unicode algorithms and IDNA processing
- **nlohmann-json** (optional): JSON serialization
- **Catch2** (tests): Testing framework

**Key advantage**: Minimal external dependencies - only requires `uni-algo` for Unicode support. All other modern C++ features (`expected`, `format`, `ranges`) are provided by the standard library!

## Code Quality Tools

- **.clang-format**: Modern C++23 formatting configuration based on Google style
- **.clang-tidy**: Comprehensive linting with bugprone, modernize, performance, and readability checks

## Continuous Integration

The library is tested on **26 build configurations** across multiple platforms and compilers to ensure broad compatibility and C++23 standards compliance.

### Tested Compilers

**Linux (12 configurations):**
- **GCC 13** - Debug + Release (pre-installed on ubuntu-24.04)
- **GCC 14** - Debug + Release (pre-installed on ubuntu-24.04)
- **Clang 18** - Debug + Release (with libc++, pre-installed)
- **Clang 19** - Debug + Release (with libc++, from LLVM repository)
- **Clang 20** - Debug + Release (with libc++, from LLVM repository)
- **Clang 21** - Debug + Release (with libc++, from LLVM repository)

**macOS (8 configurations):**
- **Clang 18** - Debug + Release (LLVM from Homebrew)
- **Clang 19** - Debug + Release (LLVM from Homebrew)
- **Clang 20** - Debug + Release (LLVM from Homebrew)
- **Clang 21** - Debug + Release (LLVM from Homebrew)

**Windows (4 configurations):**
- **MSVC 2022** - Debug + Release (Visual Studio 2022)
- **MSVC 2026** - Debug + Release (Visual Studio 2026)

### CI Implementation Details

**Linux Clang with libc++:**
- Uses custom vcpkg triplet (`x64-linux-libcxx`) to build dependencies with libc++
- Required for C++23 features (`std::expected`, `std::format`) with Clang on Linux
- Triplet configuration: `cmake/vcpkg-triplets/x64-linux-libcxx.cmake`

**Compiler Installation:**
- Pre-installed compilers used when available for faster builds
- Clang 19-21: Installed from LLVM apt repository
- macOS Clang: Installed via Homebrew (`brew install llvm@<version>`)

**Build Matrix:**
- All configurations test both Debug and RelWithDebInfo builds
- Comprehensive coverage across GCC, Clang (with libc++), and MSVC
- Tests C++23 standard library features across all platforms

## Test Results

The library has comprehensive test coverage with **excellent results**:

**Overall: 22/22 test suites passing (100%)**
**Assertions: 242/242 passing (100%)**

### Test Suite Results

✅ **Containers** (1/1)
- static_vector_tests

✅ **Unicode** (4/4)
- unicode_tests
- unicode_code_point_tests
- unicode_range_tests
- byte_conversion_tests

✅ **Domain/IDNA** (3/3)
- idna_table_tests
- punycode_tests
- domain_tests

✅ **Percent Encoding** (2/2)
- percent_decoding_tests
- percent_encoding_tests

✅ **Network** (2/2)
- ipv4_address_tests
- ipv6_address_tests

✅ **Core Parsing** (5/5)
- parse_host_tests
- url_parse_tests
- parse_path_tests
- parse_query_tests
- url_serialize_tests

✅ **URL** (3/3)
- url_vector_tests
- url_setter_tests
- url_tests

✅ **Extensions** (2/2)
- filesystem_path_tests
- json_query_tests

✅ **Allocations** (1/1)
- host_parsing_tests

### Known Issues

**No known issues** - All test suites are passing with excellent coverage.

## Migration Notes

### C++23 Modernization (2025 Reboot)

The library was recently modernized to be C++23-only, removing the legacy v1 (C++17) and v2 (C++20) implementations. Key changes:

**Namespace Simplification**:
- Removed version-specific namespaces (`skyr::v1`, `skyr::v2`, `skyr::v3`)
- All code now in main `skyr` namespace
- Directory structure simplified from `include/skyr/v3/` to `include/skyr/`

**Standard Library Migration**:
- `tl::expected<T, E>` → `std::expected<T, E>`
  - Note: `.map()` method became `.transform()` in std::expected
  - Error types must match in `.and_then()` chains (std::expected is stricter)
- `fmt::format` → `std::format`
- `range-v3` → `std::ranges`
  - `ranges::views::join` → manual join implementation (std::ranges has no join_with yet)
  - `ranges::views::split_when` → manual split with predicate
  - `ranges::actions::erase` → container `.erase()` method
  - `ranges::actions::join` → manual string concatenation

**Common Pitfalls**:
- **Namespace shadowing**: Inside `namespace skyr`, use unqualified type names (`host`, `ipv4_address`) or `::skyr::` prefix, not `skyr::` (which becomes `skyr::skyr::`)
- **Variable shadowing**: Avoid variables with the same name as types (e.g., `auto domain_name = std::string{}` shadows the `domain_name` type)
- **Range algorithm availability**: Not all range-v3 algorithms exist in std::ranges yet - may need manual implementations