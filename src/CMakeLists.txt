# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

configure_file(
        version.hpp.in
        ${skyr_SOURCE_DIR}/include/skyr/version.hpp
)

set(skyr_SRCS
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
        url/filesystem.cpp
        url/percent_encoding/errors.cpp

        ${skyr_SOURCE_DIR}/include/skyr/config.hpp
        ${skyr_SOURCE_DIR}/include/skyr/version.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/details/to_bytes.hpp
        ${skyr_SOURCE_DIR}/include/skyr/traits/string_traits.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/errors.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/core.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/traits/iterator_value.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/traits/range_iterator.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/traits/range_value.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/ranges/views/u8_view.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/ranges/views/unchecked_u8_view.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/ranges/views/u16_view.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/ranges/transforms/byte_transform.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/ranges/transforms/u16_transform.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/ranges/transforms/u32_transform.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/idna.hpp
        ${skyr_SOURCE_DIR}/include/skyr/unicode/domain.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/percent_encoding/errors.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/percent_encoding/percent_decode_range.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/percent_encoding/percent_encode_range.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/percent_encoding/percent_encoded_char.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/url_record.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/url_parse.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/url_serialize.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/url_error.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/url_search_parameters.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url/filesystem.hpp
        ${skyr_SOURCE_DIR}/include/skyr/url.hpp)

add_library(url ${skyr_SRCS})

set_property(TARGET url PROPERTY OUTPUT_NAME skyr-url)

target_link_libraries(url PUBLIC tl::expected)
if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    if (skyr_BUILD_WITH_LLVM_LIBCXX)
        target_link_libraries(url PUBLIC "c++")
    endif()
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    target_link_libraries(url PUBLIC "stdc++fs")
endif()

set_target_properties(url PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        POSITION_INDEPENDENT_CODE ON)

target_include_directories(url
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${skyr_SOURCE_DIR}/include>
    PRIVATE
        ${skyr_SOURCE_DIR}/src)

#propagate sources to parent scope for one-lib-build
set(skyr_SRCS ${skyr_SRCS} PARENT_SCOPE)
