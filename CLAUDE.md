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
- Custom Unicode/IDNA implementation (header-only)

**Key Advantages**:
- **Header-only** - just include and use, no linking required
- **Zero external dependencies** - completely self-contained for core URL parsing

## Building

### Dependencies

**Required**:
- C++23-compliant compiler (GCC 13+, Clang 16+, MSVC 2022 17.6+)

**Optional** (automatically disabled with warnings if not found):
- `catch2` for tests
- `nlohmann-json` for JSON functionality

To install optional dependencies:
```bash
cd ${VCPKG_ROOT}
./vcpkg install catch2 nlohmann-json
```

**Note**: The library is completely self-contained with zero external dependencies. Unicode/IDNA/Punycode support is built-in via custom header-only implementation.

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
- `skyr_BUILD_WPT` (OFF): Build Web Platform Tests runner
- `skyr_BUILD_BENCHMARKS` (OFF): Build performance benchmarks
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

## Web Platform Tests (WPT)

**Web Platform Tests** provide conformance testing against the official WhatWG URL specification test suite. Unlike unit tests, WPT generates a **report** showing which edge cases are handled and which are not.

### Philosophy

- **Not pass/fail tests** - WPT is a reporting tool, not a quality gate
- **Edge case discovery** - Shows which obscure URL patterns are supported
- **100% is not the goal** - Some edge cases may be intentionally unsupported if they add complexity or hurt performance
- **Separate from unit tests** - Does not affect CI/PR status

### Building WPT Tests

```bash
cmake \
  -B _build \
  -G "Ninja" \
  -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
  -Dskyr_BUILD_WPT=ON \
  .
cmake --build _build
```

**Dependencies**: Requires `nlohmann-json` for parsing test data.

### Running WPT Tests

```bash
# Run with latest test data (downloads from WPT repository)
./_build/tests/wpt/wpt_runner --force-download

# Run with cached data (if less than 24 hours old)
./_build/tests/wpt/wpt_runner

# Specify custom cache location
./_build/tests/wpt/wpt_runner --cache /path/to/urltestdata.json
```

### Report Format

The runner outputs a comprehensive report:

```
=================================================
WPT URL Test Suite Report
=================================================
Test Data: https://github.com/web-platform-tests/...
Generated: 2025-12-30 10:30:00 UTC

SUMMARY:
  Total Tests:      1,234
  Successful:       1,150 (93.2%)
  Failed:            84 (6.8%)

FAILURE BREAKDOWN:
  Parse Errors:      45 (53.6% of failures)
  Component Mismatch: 39 (46.4% of failures)

SAMPLE FAILURES (first 20):
  [1] Input: "http://example.org/foo%2€bar"
      Expected: parse failure
      Actual:   parsed successfully

  [2] Input: "http://[::1]:65536/"
      Expected: parsed successfully
      Actual:   parse failure (invalid port)
  ...
=================================================
```

### Implementation Details

- **Parallel execution**: Uses `std::execution::par_unseq` to process tests in parallel
- **Thread-safe aggregation**: Atomic counters and mutex-protected failure collection
- **Automatic downloads**: Fetches latest `urltestdata.json` from WPT repository
- **Smart caching**: Reuses cached data if less than 24 hours old

### GitHub Actions

WPT runs automatically via `.github/workflows/wpt-integration.yml`:

**Triggers:**
- Push to `main` branch
- Weekly schedule (Mondays at 00:00 UTC)
- Manual workflow dispatch

**Output:**
- Report displayed in workflow log
- Full report uploaded as artifact (90-day retention)
- Does not fail CI - informational only

**Configuration:**
- Single build: Linux GCC 14 Release
- Fast execution: Parallel test processing
- Always downloads latest test data

### Test Data Source

Test data comes from the official WPT repository:
`https://github.com/web-platform-tests/wpt/blob/master/url/resources/urltestdata.json`

This ensures compliance testing against the latest WhatWG URL specification test cases.

## Benchmarks

**Performance benchmarks** measure runtime URL parsing speed to identify optimization opportunities and track performance regressions.

### Philosophy

- **Measure, don't guess** - Profile before optimizing
- **Real-world scenarios** - Tests diverse URL patterns (ASCII, IDN, IPv6, percent-encoded, etc.)
- **Actionable metrics** - Reports average µs/URL and throughput (URLs/second)
- **Optional** - Not required for normal development (disabled by default)

### Building Benchmarks

```bash
cmake \
  -B _build \
  -G "Ninja" \
  -Dskyr_BUILD_BENCHMARKS=ON \
  .
cmake --build _build --target url_parsing_bench
```

### Running Benchmarks

```bash
# Default: 10,000 iterations × 34 URLs = 340,000 parses
./_build/benchmark/url_parsing_bench

# Custom iteration count (100,000 iterations)
./_build/benchmark/url_parsing_bench 100000

# Quick test (1,000 iterations)
./_build/benchmark/url_parsing_bench 1000
```

### Example Output

```
=================================================
URL Parsing Benchmark Results
=================================================

Configuration:
  Test URLs:     34 unique patterns
  Iterations:    10000
  Total URLs:    340000

Results:
  Total time:    820 ms
  Successful:    330000 (97.1%)
  Failed:        10000 (2.9%)

Performance:
  Average:       2.412 µs/URL
  Throughput:    414634 URLs/second

=================================================
```

### Interpreting Results

**Good performance (on modern hardware):**
- Average: < 5 µs/URL
- Throughput: > 200,000 URLs/second

**Investigate if:**
- Average: > 10 µs/URL
- Throughput: < 100,000 URLs/second

### Profiling

To find actual performance bottlenecks, use profiling tools:

**macOS (Instruments - requires Xcode):**
```bash
# First, install Xcode from App Store or https://developer.apple.com/download/
# Verify: xctrace version

cmake -B _build -G Ninja -Dskyr_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build _build --target url_parsing_bench

# Profile with xctrace (modern replacement for 'instruments' command)
xctrace record --template 'Time Profiler' \
  --output /tmp/url_bench.trace \
  --launch ./_build/benchmark/url_parsing_bench 50000

# Open results in Instruments GUI
open /tmp/url_bench.trace
```

**macOS (sample - built-in, no Xcode needed):**
```bash
cmake -B _build -G Ninja -Dskyr_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build _build --target url_parsing_bench
sample url_parsing_bench 10 -file /tmp/profile.txt &
./_build/benchmark/url_parsing_bench 50000
open /tmp/profile.txt
```

**Linux (perf):**
```bash
cmake -B _build -G Ninja -Dskyr_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build _build --target url_parsing_bench
perf record -g ./_build/benchmark/url_parsing_bench 50000
perf report
```

**All platforms (Valgrind):**
```bash
cmake -B _build -G Ninja -Dskyr_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build _build --target url_parsing_bench
valgrind --tool=callgrind ./_build/benchmark/url_parsing_bench 1000
qcachegrind callgrind.out  # macOS: brew install qcachegrind
                           # Linux: kcachegrind
```

### Test Coverage

The benchmark tests 34 diverse URL patterns:
- Simple ASCII URLs (http, https, ftp)
- URLs with query parameters and fragments
- URLs with authentication (user:pass@host)
- URLs with non-default ports
- Internationalized domain names (IDN): `http://example.إختبار/`, `https://münchen.de/`
- Unicode in paths: `http://example.com/π`, `https://example.org/文档/`
- Percent-encoded URLs: `http://example.com/path%20with%20spaces`
- Complex real-world URLs (Google search, GitHub, Wikipedia)
- IPv4 addresses: `http://192.168.1.1/`, `https://127.0.0.1:8443/`
- IPv6 addresses: `http://[::1]/`, `https://[2001:db8::1]/`
- Edge cases: file://, data:, mailto:

### Performance Expectations

**Typical results on modern hardware (Apple M1/M2, Intel i7+, AMD Ryzen):**
- Average: 2-4 µs/URL
- Throughput: 250,000 - 500,000 URLs/second

**Why this is fast enough:**
- Most applications parse URLs once per request
- A typical HTTP request takes 10-100ms
- URL parsing is < 0.01% of total request time
- Bottleneck is almost never URL parsing

### Before Adding Dependencies

Before adding external libraries like simdutf for "faster UTF conversion":

1. **Profile first** - Use profiling tools to find real bottlenecks
2. **Measure UTF time** - Is UTF conversion > 10% of runtime?
3. **Consider trade-offs** - Zero dependencies vs marginal speedup

The benchmark helps answer: "Is optimization worth the complexity?"

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
- **nlohmann-json** (optional): JSON serialization
- **Catch2** (optional, tests only): Testing framework

**Key advantage**: Zero external dependencies for core URL parsing! All modern C++ features (`expected`, `format`, `ranges`) and Unicode/IDNA support are either from the standard library or custom header-only implementations.

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