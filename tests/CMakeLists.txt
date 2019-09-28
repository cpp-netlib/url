# Copyright (c) Glyn Matthews 2012-2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


find_package(Catch2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(
        TESTS
        byte_conversion_tests
        percent_decoding_tests
        percent_encoding_tests
        url_tests
        url_web_platform_tests
        url_parsing_example_tests
        url_setter_tests
        url_search_parameters_tests
        ipv4_address_tests
        ipv6_address_tests
        idna_table_tests
        punycode_tests
        domain_tests
        unicode_tests
        unicode_code_point_tests
        unicode_range_tests
    )

if (Skyr_BUILD_FILESYSTEM_PATH_FUNCTIONS)
		LIST(APPEND TESTS filesystem_path_tests)
endif()

if (MSVC)
    # For json.hpp
    add_definitions(-D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING)
endif(MSVC)

foreach (test ${TESTS})
    add_executable(${test} ${test}.cpp)
    add_dependencies(${test} skyr-url)
    target_link_libraries(
            ${test} PRIVATE ${CMAKE_THREAD_LIBS_INIT} skyr-url Catch2::Catch2 nlohmann_json nlohmann_json::nlohmann_json)
    set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Skyr_BINARY_DIR}/tests)
    add_test(${test} ${Skyr_BINARY_DIR}/tests/${test}
    )
endforeach (test)

file(GLOB URI_LISTS *.txt *.json)
file(COPY ${URI_LISTS} DESTINATION ${Skyr_BINARY_DIR}/tests)
