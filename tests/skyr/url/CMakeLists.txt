# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (file_name
        url_tests.cpp
        url_vector_tests.cpp
        url_setter_tests.cpp
        url_search_parameters_tests.cpp
        url_sanitize_tests.cpp
        url_format_tests.cpp
        url_with_methods_tests.cpp
        wpt_conformance_tests.cpp
        )
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/url test_name)
endforeach ()

if (NOT skyr_BUILD_WITHOUT_EXCEPTIONS)
    foreach (file_name
            url_tests_with_exceptions.cpp
            url_literal_tests.cpp
            )
        skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/url test_name)
    endforeach()
endif()
