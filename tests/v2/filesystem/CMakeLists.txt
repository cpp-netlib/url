# Copyright (c) Glyn Matthews 2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(../../../cmake/skyr-url-functions.cmake)

foreach (file_name filesystem_path_tests.cpp)
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/filesystem test_name v2)
    target_link_libraries(${test_name} PRIVATE skyr-filesystem-v2)
endforeach()
