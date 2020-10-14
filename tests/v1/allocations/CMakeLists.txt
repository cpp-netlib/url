# Copyright (c) Glyn Matthews 2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(../../../cmake/skyr-url-functions.cmake)

set(full_warnings $<BOOL:${skyr_FULL_WARNINGS}>)
set(warnings_as_errors $<BOOL:${skyr_WARNINGS_AS_ERRORS}>)
set(no_exceptions $<BOOL:${skyr_BUILD_WITHOUT_EXCEPTIONS}>)
set(no_rtti $<BOOL:${skyr_BUILD_WITHOUT_RTTI}>)

set(gnu $<CXX_COMPILER_ID:GNU>)
set(clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>)
set(libcxx $<AND:${clang},$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>)
set(msvc $<CXX_COMPILER_ID:MSVC>)

foreach(
        file_name
        host_parsing_tests.cpp
)
    skyr_remove_extension(${file_name} test)
    add_executable(${test} ${file_name})
    add_dependencies(${test} skyr-url-v1)
    target_compile_options(
            ${test}
            PRIVATE
            $<$<AND:$<OR:${gnu},${clang}>,${full_warnings}>:-Wall>
            $<$<AND:$<OR:${gnu},${clang}>,${warnings_as_errors}>:-Werror>
            $<$<AND:$<OR:${gnu},${clang}>,${no_exceptions}>:-fno-exceptions>
            $<$<AND:$<OR:${gnu},${clang}>,${no_rtti}>:-fno-rtti>
            $<$<AND:$<OR:${gnu},${clang}>,$<CONFIG:MinSizeRel>>:-flto>
            $<${libcxx}:-stdlib=libc++>

            $<$<AND:${msvc},${full_warnings}>:/W4>
            $<$<AND:${msvc},${warnings_as_errors}>:/WX>
            $<$<AND:${msvc},$<NOT:${no_exceptions}>>:/EHsc>
            $<$<AND:${msvc},${no_rtti}>:/GR->
    )
    target_link_libraries(
            ${test}
            PRIVATE
            skyr-url-v1
            fmt::fmt
    )
    set_target_properties(
            ${test}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests/allocations/
    )
endforeach()
