# Copyright (c) Glyn Matthews 2012-2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(
        URL_TESTS

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
    LIST(APPEND
            URL_TESTS
            filesystem_path_tests.cpp)
endif()

if (NOT Skyr_BUILD_WITHOUT_EXCEPTIONS)
    LIST(APPEND URL_TESTS
            url_tests_with_exceptions.cpp
            url_web_platform_tests_with_exceptions.cpp
            )
endif()

foreach (file_name ${URL_TESTS})
    _create_test(${file_name} ${Skyr_BINARY_DIR}/tests/url test)
    add_test(${test} ${Skyr_BINARY_DIR}/tests/url/${test})
endforeach ()

file(GLOB URL_TEST_DATA *.txt *.json)
file(COPY ${URL_TEST_DATA} DESTINATION ${Skyr_BINARY_DIR}/tests/url)
