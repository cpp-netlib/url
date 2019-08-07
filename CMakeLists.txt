# Copyright (c) Glyn Matthews 2012-2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


cmake_minimum_required(VERSION 3.8)
project(Skyr)

option(Skyr_BUILD_TESTS "Build the URL tests." ON)
option(Skyr_BUILD_DOCS "Build the URL documentation." OFF)
option(Skyr_BUILD_EXAMPLES "Build the URL examples." OFF)
option(Skyr_FULL_WARNINGS "Build the library with all warnings turned on." ON)
option(Skyr_WARNINGS_AS_ERRORS "Treat warnings as errors." ON)
option(Skyr_USE_STATIC_CRT "Use static C Runtime library (/MT or MTd)." ON)
option(Skyr_BUILD_FILESYSTEM_PATH_FUNCTIONS "Build the filesystem path functions" ON)

find_package(Threads REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE true)

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

    if (Skyr_FULL_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    endif()

    if (Skyr_WARNINGS_AS_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    endif()

    message("C++ Flags: ${CMAKE_CXX_FLAGS} link flags: ${CMAKE_CXX_LINK_FLAGS}")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    if (NOT Skyr_DISABLE_LIBCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()

    if (Skyr_FULL_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    endif()

    if (Skyr_WARNINGS_AS_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    endif()

    message("C++ Flags: ${CMAKE_CXX_FLAGS} link flags: ${CMAKE_CXX_LINK_FLAGS}")
endif()


if (MSVC)
    if(DEFINED MSVC_VERSION AND MSVC_VERSION LESS 1900)
        message(FATAL_ERROR "Requires VS 2015 or later")
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

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

    add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

add_subdirectory(src)

# Testing
if (Skyr_BUILD_TESTS)
    message(STATUS "Configuring tests")
    enable_testing()
    set(BUILD_GMOCK OFF)
    set(BUILD_GTEST ON)
    set(INSTALL_GMOCK OFF)
    set(INSTALL_GTEST OFF)
    add_subdirectory(deps/googletest)
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
