# Custom vcpkg triplet for Linux x64 with Clang and libc++
# This triplet is used for building packages with libc++ instead of libstdc++
# Required for C++23 std::expected, std::format support with Clang on Linux

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)

# Use libc++ instead of libstdc++
# CC and CXX environment variables must be set to clang-18/clang++-18
set(VCPKG_CXX_FLAGS "-stdlib=libc++")
set(VCPKG_C_FLAGS "")
set(VCPKG_LINKER_FLAGS "-stdlib=libc++ -lc++abi")