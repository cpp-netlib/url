# Copyright (c) Glyn Matthews 2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(
        UNICODE_TESTS

        # Unicode
        unicode_tests.cpp
        unicode_code_point_tests.cpp
        unicode_range_tests.cpp
        idna_table_tests.cpp
        punycode_tests.cpp
        domain_tests.cpp
    )

foreach (file_name ${UNICODE_TESTS})
    _create_test(${file_name} ${Skyr_BINARY_DIR}/tests/unicode test)
    add_test(${test} ${Skyr_BINARY_DIR}/tests/unicode/${test})
endforeach ()
