# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)

find_package(Catch2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

set(clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>)
set(libcxx $<AND:${clang},$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>)

function(skyr_create_test file_name output_dir test_name)
    skyr_remove_extension(${file_name} test)
    add_executable(${test})
    target_sources(${test} PRIVATE ${test}.cpp)
    target_compile_options(${test} PRIVATE $<${libcxx}:-stdlib=libc++>)
    add_dependencies(${test} skyr-url)
    target_link_libraries(
        ${test}
        PRIVATE
            skyr-url
            Catch2::Catch2
            fmt::fmt
    )
    set_target_properties(
        ${test}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${output_dir}
    )
    add_test(${test} ${output_dir}/${test})
    set(test_name ${test} PARENT_SCOPE)
endfunction()

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
