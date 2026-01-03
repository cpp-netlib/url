# Copyright (c) Glyn Matthews 2012-2021.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


cmake_minimum_required(VERSION 3.21)

get_directory_property(skyr_is_subproject PARENT_DIRECTORY)

if(NOT skyr_is_subproject)
    set(skyr_IS_TOP_LEVEL_PROJECT YES)
else()
    set(skyr_IS_TOP_LEVEL_PROJECT NO)
endif()

project(
        skyr-url
        VERSION 3.0.0
        HOMEPAGE_URL https://cpp-netlib.github.io/url
        DESCRIPTION "A C++ library that implements the WhatWG URL specification"
        LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(skyr_BUILD_TESTS "Build the URL tests." ON)
option(skyr_BUILD_WPT "Build Web Platform Tests runner (report-only, not unit tests)." OFF)
option(skyr_BUILD_DOCS "Build the URL documentation." OFF)
option(skyr_BUILD_EXAMPLES "Build the URL examples." OFF)
option(skyr_BUILD_BENCHMARKS "Build performance benchmarks." OFF)
option(skyr_FULL_WARNINGS "Build the library with all warnings turned on." ON)
option(skyr_WARNINGS_AS_ERRORS "Treat warnings as errors." ON)
option(skyr_BUILD_WITHOUT_EXCEPTIONS "Build without exceptions." OFF)
option(skyr_BUILD_WITHOUT_RTTI "Build without RTTI." OFF)
option(skyr_USE_STATIC_CRT "Use static C Runtime library (/MT or MTd)." ON)
option(skyr_BUILD_WITH_LLVM_LIBCXX "Instruct Clang to use LLVM's implementation of C++ standard library" OFF)
option(skyr_ENABLE_SANITIZERS "Enable sanitizers (address, undefined, etc.) for tests and examples" OFF)
option(skyr_CXX_STANDARD_LIBRARY "Path to non-system C++ standard library" "")

if (skyr_IS_TOP_LEVEL_PROJECT)
    set(CMAKE_VERBOSE_MAKEFILE true)
    set(CMAKE_CXX_STANDARD 23)
    if (skyr_CXX_STANDARD_LIBRARY)
        include_directories(SYSTEM ${skyr_CXX_STANDARD_LIBRARY})
        message(STATUS ${skyr_CXX_STANDARD_LIBRARY})
    endif()
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# JSON functions are optional and only available if nlohmann_json is found
find_package(nlohmann_json CONFIG)
if (NOT nlohmann_json_FOUND)
    message(WARNING "nlohmann_json not found. Install with: vcpkg install nlohmann-json")
    message(WARNING "JSON functions will be disabled.")
    set(skyr_HAS_JSON_SUPPORT OFF)
else()
    set(skyr_HAS_JSON_SUPPORT ON)
endif()

if (skyr_USE_STATIC_CRT AND (CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR CMAKE_CXX_SIMULATE_ID MATCHES "MSVC"))
    include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)
    skyr_replace_dynamic_msvcrt_linker_flags()
endif()

set(full_warnings $<BOOL:${skyr_FULL_WARNINGS}>)
set(warnings_as_errors $<BOOL:${skyr_WARNINGS_AS_ERRORS}>)
set(no_exceptions $<BOOL:${skyr_BUILD_WITHOUT_EXCEPTIONS}>)
set(no_rtti $<BOOL:${skyr_BUILD_WITHOUT_RTTI}>)
set(enable_sanitizers $<BOOL:${skyr_ENABLE_SANITIZERS}>)

set(gnu $<CXX_COMPILER_ID:GNU>)
set(clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>)
set(libcxx $<AND:${clang},$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>)
set(clang_with_gnu_stdlib $<AND:${clang},$<NOT:$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>>)
set(msvc $<CXX_COMPILER_ID:MSVC>)

# Header-only library - target definitions in cmake/targets
add_subdirectory(cmake/targets)

# Testing
if (skyr_BUILD_TESTS)
    message(STATUS "[skyr-url] Configuring tests")
    find_package(Catch2 3 CONFIG REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()

# Sanitizer tests (independent, no Catch2 needed)
if (skyr_ENABLE_SANITIZERS)
    message(STATUS "[skyr-url] Configuring sanitizer tests")
    if (NOT skyr_BUILD_TESTS)
        enable_testing()  # Only call this if not already enabled
    endif()
    add_subdirectory(tests/sanitizers)
endif()

# Documentation
if (skyr_BUILD_DOCS)
    message(STATUS "[skyr-url] Configuring documentation")
    add_subdirectory(docs)
endif()

# Examples
if (skyr_BUILD_EXAMPLES)
    message(STATUS "[skyr-url] Configuring examples")
    add_subdirectory(examples)
endif()

# Benchmarks
if (skyr_BUILD_BENCHMARKS)
    message(STATUS "[skyr-url] Configuring benchmarks")
    add_subdirectory(benchmark)
endif()

## Install
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        INSTALL_DESTINATION "share/cmake/${PROJECT_NAME}")

write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT)

set(skyr_TARGETS skyr-url skyr-filesystem)
if (skyr_HAS_JSON_SUPPORT)
    list(APPEND skyr_TARGETS skyr-json)
endif()

install(TARGETS ${skyr_TARGETS}
        EXPORT ${PROJECT_NAME}-targets
        INCLUDES DESTINATION "${CMAKE_INSTALL_DATADIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(EXPORT ${PROJECT_NAME}-targets
        DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
        NAMESPACE skyr::
        FILE "${PROJECT_NAME}-targets.cmake")

install(FILES
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}")

install(
        DIRECTORY
        "${PROJECT_SOURCE_DIR}/include/skyr/core"
        "${PROJECT_SOURCE_DIR}/include/skyr/domain"
        "${PROJECT_SOURCE_DIR}/include/skyr/network"
        "${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding"
        "${PROJECT_SOURCE_DIR}/include/skyr/unicode"
        "${PROJECT_SOURCE_DIR}/include/skyr/containers"
        "${PROJECT_SOURCE_DIR}/include/skyr/concepts"
        "${PROJECT_SOURCE_DIR}/include/skyr/platform"
        DESTINATION
        include/skyr
)

install(
        FILES
        "${PROJECT_BINARY_DIR}/include/skyr/version.hpp"
        "${PROJECT_SOURCE_DIR}/include/skyr/config.hpp"
        "${PROJECT_SOURCE_DIR}/include/skyr/url.hpp"
        "${PROJECT_SOURCE_DIR}/include/skyr/url_search_parameters.hpp"
        DESTINATION
        include/skyr
)

# Filesystem functions are always installed (C++23 guarantees std::filesystem)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/skyr/filesystem" DESTINATION include/skyr)

# JSON functions are only installed if nlohmann_json is available
if (skyr_HAS_JSON_SUPPORT)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/skyr/json" DESTINATION include/skyr)
endif()
