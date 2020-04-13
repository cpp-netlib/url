# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (file_name
        url_tests.cpp
        url_vector_tests.cpp
        url_parsing_example_tests.cpp
        url_setter_tests.cpp
        url_search_parameters_tests.cpp
        query_parameter_range_tests.cpp
        url_literal_tests.cpp
        )
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/url test)
    add_test(${test} ${PROJECT_BINARY_DIR}/tests/url/${test})
endforeach ()

if (NOT skyr_BUILD_WITHOUT_EXCEPTIONS)
    skyr_create_test(url_tests_with_exceptions.cpp ${PROJECT_BINARY_DIR}/tests/url test)
    add_test(${test} ${PROJECT_BINARY_DIR}/tests/url/${test})
endif()

file(GLOB URL_TEST_DATA *.txt *.json)
file(COPY ${URL_TEST_DATA} DESTINATION ${PROJECT_BINARY_DIR}/tests/url)
