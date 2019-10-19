# Copyright (c) Glyn Matthews 2012-2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


find_package(Catch2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

if (MSVC)
    # For json.hpp
    add_definitions(-D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING)
endif(MSVC)

set(
        TESTS

        # Unicode
        unicode_tests.cpp
        unicode_code_point_tests.cpp
        unicode_range_tests.cpp
        idna_table_tests.cpp
        punycode_tests.cpp
        domain_tests.cpp

        # URL
        byte_conversion_tests.cpp
        percent_decoding_tests.cpp
        percent_encoding_tests.cpp
        url_tests.cpp
        url_web_platform_tests.cpp
        url_parsing_example_tests.cpp
        url_setter_tests.cpp
        url_search_parameters_tests.cpp
        url_search_parameter_range_tests.cpp
        ipv4_address_tests.cpp
        ipv6_address_tests.cpp
    )

if (Skyr_BUILD_FILESYSTEM_PATH_FUNCTIONS)
    LIST(APPEND TESTS
            filesystem_path_tests.cpp)
endif()

if (NOT Skyr_BUILD_WITHOUT_EXCEPTIONS)
    LIST(APPEND TESTS
            url_tests_with_exceptions.cpp
            url_web_platform_tests_with_exceptions.cpp)
endif()

function(_remove_extension file_name basename)
    string(REGEX REPLACE "\\.[^.]*$" "" _basename ${file_name})
    set(${basename} ${_basename} PARENT_SCOPE)
endfunction()

function(_create_test file_name test_name)
    _remove_extension(${file_name} test)
    add_executable(${test} ${test}.cpp)
    add_dependencies(${test} Skyr::skyr-url)
    target_link_libraries(
            ${test} PRIVATE ${CMAKE_THREAD_LIBS_INIT} Skyr::skyr-url Catch2::Catch2 nlohmann_json nlohmann_json::nlohmann_json)
    set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Skyr_BINARY_DIR}/tests)
    set(${test_name} ${test} PARENT_SCOPE)
endfunction()

foreach (file_name ${TESTS})
    _create_test(${file_name} test)
    add_test(${test} ${Skyr_BINARY_DIR}/tests/${test})
endforeach ()

file(GLOB URI_LISTS *.txt *.json)
file(COPY ${URI_LISTS} DESTINATION ${Skyr_BINARY_DIR}/tests)
