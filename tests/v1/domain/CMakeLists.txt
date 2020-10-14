# Copyright (c) Glyn Matthews 2019-20.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (
        file_name
        idna_table_tests.cpp
        punycode_tests.cpp
        domain_tests.cpp)
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/domain test_name v1)
endforeach ()
