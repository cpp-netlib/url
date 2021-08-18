# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)

configure_file(
        version.hpp.in
        ${PROJECT_BINARY_DIR}/include/skyr/version.hpp
)

if (skyr_BUILD_V1)
    add_subdirectory(v1)
endif()
if (skyr_BUILD_V2)
    add_subdirectory(v2)
endif()
