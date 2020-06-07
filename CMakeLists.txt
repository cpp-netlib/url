# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


cmake_minimum_required(VERSION 3.14)

project(
        skyr-url
        VERSION 1.11.0
        HOMEPAGE_URL https://cpp-netlib.github.io/url
        DESCRIPTION "A C++ library that implements the WhatWG URL specification"
        LANGUAGES CXX
)

option(skyr_BUILD_TESTS "Build the URL tests." ON)
option(skyr_BUILD_DOCS "Build the URL documentation." OFF)
option(skyr_BUILD_EXAMPLES "Build the URL examples." OFF)
option(skyr_FULL_WARNINGS "Build the library with all warnings turned on." ON)
option(skyr_WARNINGS_AS_ERRORS "Treat warnings as errors." ON)
option(skyr_BUILD_WITHOUT_EXCEPTIONS "Build without exceptions." OFF)
option(skyr_BUILD_WITHOUT_RTTI "Build without RTTI." OFF)
option(skyr_USE_STATIC_CRT "Use static C Runtime library (/MT or MTd)." ON)
option(skyr_BUILD_WITH_LLVM_LIBCXX "Instruct Clang to use LLVM's implementation of C++ standard library" OFF)
option(skyr_ENABLE_FILESYSTEM_FUNCTIONS "Enable functions to convert URL to std::filesystem::path" ON)
option(skyr_ENABLE_JSON_FUNCTIONS "Enable functions to convert URL components to JSON" ON)

set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(tl-expected CONFIG REQUIRED)

if (skyr_ENABLE_JSON_FUNCTIONS)
    find_package(nlohmann_json CONFIG REQUIRED)
endif()

if (skyr_USE_STATIC_CRT AND ${CMAKE_CXX_COMPILER_ID} MATCHES MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)
    skyr_replace_dynamic_msvcrt_linker_flags()
endif()


add_subdirectory(src)

# Testing
if (skyr_BUILD_TESTS)
    message(STATUS "Configuring tests")
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
if (skyr_BUILD_DOCS)
    message(STATUS "Configuring documentation")
    add_subdirectory(docs)
endif()

# Examples
if (skyr_BUILD_EXAMPLES)
    message(STATUS "Configuring examples")
    add_subdirectory(examples)
endif()

# Add an alias for projects that use skyr-url as a dependency
if (NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(skyr::skyr-url ALIAS skyr-url)
    if (skyr_ENABLE_FILESYSTEM_FUNCTIONS)
        add_library(skyr::skyr-filesystem ALIAS skyr-filesystem)
    endif()
    if (skyr_ENABLE_JSON_FUNCTIONS)
        add_library(skyr::skyr-json ALIAS skyr-json)
    endif()
endif()

# Install
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

set(skyr_TARGETS skyr-url)
if (skyr_ENABLE_FILESYSTEM_FUNCTIONS)
    list(APPEND skyr_TARGETS skyr-filesystem)
endif()
if (skyr_ENABLE_JSON_FUNCTIONS)
    list(APPEND skyr_TARGETS skyr-json)
endif()

install(TARGETS ${skyr_TARGETS}
        EXPORT ${PROJECT_NAME}-targets
        INCLUDES DESTINATION "${CMAKE_INSTALL_DATADIR}"
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)

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
        "${PROJECT_SOURCE_DIR}/include/skyr/query"
        DESTINATION
        include/skyr
)

install(
        DIRECTORY
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/core"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/domain"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/network"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/platform"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/query"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/ranges"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/string"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/traits"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode"
        DESTINATION
        include/skyr/v1
)

install(
        FILES
        "${PROJECT_BINARY_DIR}/include/skyr/version.hpp"
        "${PROJECT_SOURCE_DIR}/include/skyr/config.hpp"
        "${PROJECT_SOURCE_DIR}/include/skyr/url.hpp"
        DESTINATION
        include/skyr
)

install(
        FILES
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/url.hpp"
        "${PROJECT_SOURCE_DIR}/include/skyr/v1/url_search_parameters.hpp"
        DESTINATION
        include/skyr/v1
)

if (skyr_ENABLE_FILESYSTEM_FUNCTIONS)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/skyr/filesystem" DESTINATION include/skyr)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/skyr/v1/filesystem" DESTINATION include/skyr/v1)
endif()

if (skyr_ENABLE_JSON_FUNCTIONS)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/skyr/json" DESTINATION include/skyr)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/skyr/v1/json" DESTINATION include/skyr/v1)
endif()
