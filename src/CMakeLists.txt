# Copyright (c) Glyn Matthews 2012-2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(Skyr_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/url_parser_context.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_parser_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_record.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ipv4_address.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ipv4_address.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ipv6_address.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ipv6_address.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/percent_encode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/unicode_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/unicode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/idna_table.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/idna_table.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/algorithms.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/domain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_parse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_parse_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_serialize.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_schemes.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_schemes.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_error.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_search_parameters.cpp

        ${Skyr_SOURCE_DIR}/include/skyr/details/to_bytes.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/external/tl/optional.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/external/tl/expected.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/config.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/optional.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/expected.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/string_traits.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/percent_encode.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/unicode.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/domain.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url_record.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url_parse.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url_serialize.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url_error.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url_search_parameters.hpp)

if (Skyr_BUILD_FILESYSTEM_PATH_FUNCTIONS)
		LIST(APPEND Skyr_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/filesystem.cpp)
		LIST(APPEND Skyr_SRCS ${Skyr_SOURCE_DIR}/include/skyr/filesystem.hpp)
endif()

add_library(skyr-url ${Skyr_SRCS})
target_link_libraries(skyr-url)
if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    if (NOT Skyr_DISABLE_LIBCXX)
        target_link_libraries(skyr-url "c++")
    endif()
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
	if (Skyr_BUILD_FILESYSTEM_PATH_FUNCTIONS)
	    target_link_libraries(skyr-url "stdc++fs")
	endif()
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU OR
    ${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations")
endif()

#propagate sources to parent scope for one-lib-build
set(Skyr_SRCS ${Skyr_SRCS} PARENT_SCOPE)

# Install
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include DESTINATION ".")
install(TARGETS skyr-url
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)
