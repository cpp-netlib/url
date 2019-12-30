# Copyright (c) Glyn Matthews 2012-2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


cmake_minimum_required(VERSION 3.8)
project(Skyr VERSION 1.3.0 DESCRIPTION "A C++ library that implements the WhatWG URL specification")

option(Skyr_BUILD_TESTS "Build the URL tests." ON)
option(Skyr_BUILD_DOCS "Build the URL documentation." OFF)
option(Skyr_BUILD_EXAMPLES "Build the URL examples." OFF)
option(Skyr_FULL_WARNINGS "Build the library with all warnings turned on." ON)
option(Skyr_WARNINGS_AS_ERRORS "Treat warnings as errors." ON)
option(Skyr_BUILD_WITHOUT_EXCEPTIONS "Build without exceptions." OFF)
option(Skyr_USE_STATIC_CRT "Use static C Runtime library (/MT or MTd)." ON)
option(Skyr_BUILD_FILESYSTEM_PATH_FUNCTIONS "Build the filesystem path functions" OFF)
option(Skyr_DISABLE_LIBCXX "Instruct Clang to use LLVM's implementation of C++ standard library" ON)

set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(tl-expected CONFIG REQUIRED)

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU OR
    ${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    if (Skyr_FULL_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    endif()

    if (Skyr_WARNINGS_AS_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    endif()

    if (Skyr_BUILD_WITHOUT_EXCEPTIONS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES MSVC)
    if(DEFINED MSVC_VERSION AND MSVC_VERSION LESS 1900)
        message(FATAL_ERROR "Requires VS 2017 or later")
    endif()

    if (Skyr_USE_STATIC_CRT)
        # Replace dynamic MSVCRT linker flags with static version.
        foreach(flag_var
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MD")
        endforeach(flag_var)
    endif(Skyr_USE_STATIC_CRT)

    add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    if (NOT Skyr_DISABLE_LIBCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

add_subdirectory(src)

function(_remove_extension file_name basename)
    string(REGEX REPLACE "\\.[^.]*$" "" _basename ${file_name})
    set(${basename} ${_basename} PARENT_SCOPE)
endfunction()

# Testing
if (Skyr_BUILD_TESTS)
    message(STATUS "Configuring tests")
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
if (Skyr_BUILD_DOCS)
    message(STATUS "Configuring documentation")
    add_subdirectory(docs)
endif()

# Examples
if (Skyr_BUILD_EXAMPLES)
    message(STATUS "Configuring examples")
    add_subdirectory(examples)
endif()
