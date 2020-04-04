# Copyright (c) Glyn Matthews 2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(FetchContent)

FetchContent_Declare(
        wpt
        GIT_REPOSITORY https://github.com/web-platform-tests/wpt
        GIT_TAG ${skyr_WPT_TAG}
)
FetchContent_MakeAvailable(wpt)

SET(skyr_WPT_TESTS
        url_web_platform_tests.cpp
#        url_wpt_setter_tests.cpp
        )

if (NOT skyr_BUILD_WITHOUT_EXCEPTIONS)
    LIST(APPEND skyr_WPT_TESTS
            url_web_platform_tests_with_exceptions.cpp
            )
endif()

foreach (file_name ${skyr_WPT_TESTS})
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/url/wpt test)
    add_test(${test} ${PROJECT_BINARY_DIR}/tests/url/wpt/${test})
endforeach ()

file(GLOB WPT_TEST_DATA ${wpt_SOURCE_DIR}/url/resources/*.json)
file(COPY ${WPT_TEST_DATA} DESTINATION ${PROJECT_BINARY_DIR}/tests/url/wpt)
