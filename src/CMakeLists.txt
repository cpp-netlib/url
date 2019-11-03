# Copyright (c) Glyn Matthews 2012-2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(Skyr_SRCS
		unicode/errors.cpp
        unicode/idna.cpp
		unicode/domain.cpp
		url/url_parser_context.hpp
		url/url_parser_context.cpp
		url/url_record.cpp
		url/ipv4_address.cpp
		url/ipv4_address.hpp
		url/ipv6_address.cpp
		url/ipv6_address.hpp
		url/algorithms.hpp
		url/url_parse.cpp
		url/url_parse_impl.hpp
		url/url_serialize.cpp
		url/url_schemes.hpp
		url/url_schemes.cpp
		url/url.cpp
		url/url_error.cpp
		url/url_search_parameters.cpp
		url/percent_encoding/errors.cpp

        ${Skyr_SOURCE_DIR}/include/skyr/url/details/to_bytes.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/config.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/traits/string_traits.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/errors.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/core.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/traits/iterator_value.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/traits/range_iterator.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/traits/range_value.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/ranges/views/u8_view.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/ranges/views/unchecked_u8_view.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/ranges/views/u16_view.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/ranges/transforms/byte_transform.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/ranges/transforms/u16_transform.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/ranges/transforms/u32_transform.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/idna.hpp
		${Skyr_SOURCE_DIR}/include/skyr/unicode/domain.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url/percent_encoding/errors.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url/percent_encoding/percent_decode_range.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url/percent_encoding/percent_encode_range.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url/percent_encoding/percent_encoded_char.hpp
		${Skyr_SOURCE_DIR}/include/skyr/url/url_record.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url/url_parse.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url/url_serialize.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url/url_error.hpp
        ${Skyr_SOURCE_DIR}/include/skyr/url/url_search_parameters.hpp
		${Skyr_SOURCE_DIR}/include/skyr/url.hpp)

if (Skyr_BUILD_FILESYSTEM_PATH_FUNCTIONS)
		LIST(APPEND Skyr_SRCS url/filesystem.cpp)
		LIST(APPEND Skyr_SRCS ${Skyr_SOURCE_DIR}/include/skyr/url/filesystem.hpp)
endif()

add_library(skyr-url ${Skyr_SRCS})

target_link_libraries(skyr-url tl::expected)
if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    if (NOT Skyr_DISABLE_LIBCXX)
        target_link_libraries(skyr-url "c++")
    endif()
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
	if (Skyr_BUILD_FILESYSTEM_PATH_FUNCTIONS)
		target_link_libraries(skyr-url "stdc++fs")
	endif()
endif()

set_target_properties(skyr-url PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION 1
		POSITION_INDEPENDENT_CODE ON)

add_library(Skyr::skyr-url ALIAS skyr-url)

target_include_directories(skyr-url
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${Skyr_SOURCE_DIR}/include>
    PRIVATE
        ${Skyr_SOURCE_DIR}/src)

#propagate sources to parent scope for one-lib-build
set(Skyr_SRCS ${Skyr_SRCS} PARENT_SCOPE)

# Install
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include DESTINATION ".")
install(TARGETS skyr-url
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)
