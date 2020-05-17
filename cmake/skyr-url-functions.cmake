# Copyright (c) Glyn Matthews 2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


include(CheckCXXSourceCompiles)


function(skyr_replace_dynamic_msvcrt_linker_flags)
    # Replace dynamic MSVCRT linker flags with static version.
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
endfunction()

function(skyr_remove_extension file_name basename)
    string(REGEX REPLACE "\\.[^.]*$" "" _basename ${file_name})
    set(${basename} ${_basename} PARENT_SCOPE)
endfunction()


function(skyr_check_filesystem compile_definitions)
    check_cxx_source_compiles("#include <filesystem>
int main() { std::filesystem::path p{}; }" SKYR_USE_CXX17_FILESYSTEM)
    if (SKYR_USE_CXX17_FILESYSTEM)
        set(_compile_definitions "-DSKYR_USE_CXX17_FILESYSTEM")
    else()
        check_cxx_source_compiles("#include <experimental/filesystem>
int main() { std::experimental::filesystem::path p{}; }" SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)
        if (SKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM)
            set(_compile_definitions "-DSKYR_USE_CXX17_EXPERIMENTAL_FILESYSTEM")
            if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU OR ${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
                set(_compile_definitions "${_compile_definitions} -D_LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM")
            elseif (${CMAKE_CXX_COMPILER_ID} MATCHES MSVC)
                set(_compile_definitions "${_compile_definitions} -D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING")
            endif()
        else()
            message(FATAL_ERROR "Filesystem operations are not supported")
        endif()
    endif()

    set(${compile_definitions} ${_compile_definitions} PARENT_SCOPE)
endfunction()
