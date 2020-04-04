# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(
        skyr_URL_TESTS

        # URL
        byte_conversion_tests.cpp
        percent_decoding_tests.cpp
        percent_encoding_tests.cpp
        url_tests.cpp
        url_vector_tests.cpp
        url_parsing_example_tests.cpp
        url_setter_tests.cpp
        url_search_parameters_tests.cpp
        url_search_parameter_range_tests.cpp
        ipv4_address_tests.cpp
        ipv6_address_tests.cpp
        url_literal_tests.cpp
        filesystem_path_tests.cpp
    )

if (NOT skyr_BUILD_WITHOUT_EXCEPTIONS)
    LIST(APPEND URL_TESTS
            url_tests_with_exceptions.cpp
            )
endif()

foreach (file_name ${skyr_URL_TESTS})
    skyr_create_test(${file_name} ${skyr_BINARY_DIR}/tests/url test)
    add_test(${test} ${skyr_BINARY_DIR}/tests/url/${test})
endforeach ()

if (${skyr_BUILD_WPT_TESTS})
    add_subdirectory(wpt)
endif()

file(GLOB URL_TEST_DATA *.txt *.json)
file(COPY ${URL_TEST_DATA} DESTINATION ${skyr_BINARY_DIR}/tests/url)
