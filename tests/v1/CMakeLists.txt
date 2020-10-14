# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)

find_package(Catch2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

set(clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>)
set(libcxx $<AND:${clang},$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>)

add_subdirectory(containers)
add_subdirectory(unicode)
add_subdirectory(domain)
add_subdirectory(percent_encoding)
add_subdirectory(network)
add_subdirectory(url)
if (skyr_ENABLE_FILESYSTEM_FUNCTIONS)
    add_subdirectory(filesystem)
endif()
if (skyr_ENABLE_JSON_FUNCTIONS)
    add_subdirectory(json)
endif()
add_subdirectory(allocations)
