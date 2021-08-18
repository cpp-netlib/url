# Copyright (c) Glyn Matthews 2018-20.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)

foreach(
        file_name
        example_01.cpp
        example_02.cpp
        example_03.cpp
        example_04.cpp
        example_05.cpp
        example_06.cpp
        example_07.cpp
        example_08.cpp
        example_09.cpp
        example_10.cpp
        example_11.cpp
)
    skyr_remove_extension(${file_name} example)
    add_executable(${example} ${file_name})
    add_dependencies(${example} skyr-url-v1)
    target_compile_options(
            ${example}
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
            ${example}
            PRIVATE
            skyr-url-v1
    )
    set_target_properties(
            ${example}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/examples
    )
endforeach()
