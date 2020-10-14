# Copyright (c) Glyn Matthews 2019-20.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (
        file_name
        unicode_tests.cpp
        unicode_code_point_tests.cpp
        unicode_range_tests.cpp
        byte_conversion_tests.cpp)
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/unicode test_name v1)
endforeach ()
