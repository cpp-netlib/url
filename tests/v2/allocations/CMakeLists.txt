# Copyright (c) Glyn Matthews 2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(../../../cmake/skyr-url-functions.cmake)

foreach(
        file_name
        host_parsing_tests.cpp
)
    skyr_remove_extension(${file_name} basename)
    set(test ${basename}-v2)
    add_executable(${test} ${file_name})
    add_dependencies(${test} skyr-url-v2)
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
            skyr-url-v2
            fmt::fmt
    )
    set_target_properties(
            ${test}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests/allocations/
    )
endforeach()
