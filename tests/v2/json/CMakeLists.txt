# Copyright (c) Glyn Matthews 2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

if(${CMAKE_CXX_COMPILER_ID} MATCHES MSVC)
    # For json.hpp
    add_definitions(-D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING)
endif()

foreach (file_name json_query_tests.cpp)
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/json test_name v2)
    target_link_libraries(${test_name} PRIVATE skyr-json-v2)
endforeach()
