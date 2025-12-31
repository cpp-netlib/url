# URL Parsing Benchmarks

Performance benchmarks for the skyr-url library.

## Building

Enable benchmarks during CMake configuration:

```bash
cmake -B _build -G Ninja -Dskyr_BUILD_BENCHMARKS=ON
cmake --build _build --target url_parsing_bench
```

## Running

### Basic benchmark

```bash
./_build/benchmark/url_parsing_bench
```

Default: 10,000 iterations × 34 URLs = 340,000 parses

### Custom iteration count

```bash
# Run 100,000 iterations (3.4M URLs)
./_build/benchmark/url_parsing_bench 100000

# Quick test with 1,000 iterations
./_build/benchmark/url_parsing_bench 1000
```

## Profiling

### macOS (with Xcode Instruments)

**Requires Xcode** - Install from App Store or https://developer.apple.com/download/

```bash
# Verify Xcode tools are installed
xctrace version

# Build with debug symbols
cmake -B _build -G Ninja \
  -Dskyr_BUILD_BENCHMARKS=ON \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo

cmake --build _build --target url_parsing_bench

# Profile with xctrace (modern replacement for 'instruments' command)
xctrace record --template 'Time Profiler' \
  --output /tmp/url_bench.trace \
  --launch ./_build/benchmark/url_parsing_bench 50000

# Open results in Instruments GUI
open /tmp/url_bench.trace
```

### macOS (with sample - no Xcode needed)

**Built-in** - No installation required

```bash
# Build with debug symbols
cmake -B _build -G Ninja \
  -Dskyr_BUILD_BENCHMARKS=ON \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo

cmake --build _build --target url_parsing_bench

# Profile with sample (10 second sample)
sample url_parsing_bench 10 -file /tmp/profile.txt &
./_build/benchmark/url_parsing_bench 50000

# View results
open /tmp/profile.txt
```

### Linux (with perf)

```bash
# Build with debug symbols
cmake -B _build -G Ninja \
  -Dskyr_BUILD_BENCHMARKS=ON \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo

cmake --build _build --target url_parsing_bench

# Profile with perf
perf record -g ./_build/benchmark/url_parsing_bench 50000
perf report

# Or generate flamegraph
perf script | stackcollapse-perf.pl | flamegraph.pl > url_parsing.svg
```

### All platforms (with Valgrind callgrind)

```bash
# Profile with callgrind
valgrind --tool=callgrind --callgrind-out-file=callgrind.out \
  ./_build/benchmark/url_parsing_bench 1000

# Visualize with kcachegrind (Linux) or qcachegrind (macOS)
qcachegrind callgrind.out
```

## Understanding Results

The benchmark tests 34 diverse URL patterns:
- Simple ASCII URLs
- URLs with query parameters and fragments
- Internationalized domain names (IDN)
- Unicode in paths
- Percent-encoded URLs
- IPv4 and IPv6 addresses
- Edge cases (data URIs, mailto, etc.)

### Typical Performance

On modern hardware (Apple M1/M2, Intel i7/i9, AMD Ryzen):
- **Average:** 2-4 µs/URL
- **Throughput:** 250,000 - 500,000 URLs/second

### Interpreting Results

**Good performance:**
- < 5 µs/URL average
- > 200,000 URLs/second throughput

**Investigate if:**
- > 10 µs/URL average
- < 100,000 URLs/second throughput

Use profiling to identify hotspots before optimization.

## Adding New Test Cases

Edit `url_parsing_bench.cpp` and add URLs to the `test_urls` vector:

```cpp
const std::vector<std::string> test_urls = {
    // Your new test URLs here
    "http://your-test-url.com/path",
    // ...
};
```

Rebuild and re-run the benchmark.