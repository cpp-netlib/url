# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-git.cmake)

skyr_get_git_sha1(skyr-url_VERSION_SHA1)

configure_file(
        version.hpp.in
        ${PROJECT_BINARY_DIR}/include/skyr/version.hpp
)

#################################################
# skyr-url
#################################################

add_library(
        skyr-url
)

target_sources(skyr-url
    PRIVATE

        v1/core/url_parser_context.hpp
        v1/core/url_parser_context.cpp
        v1/core/parse.cpp
        v1/core/url_parse_impl.hpp
        v1/core/serialize.cpp
        v1/domain/domain.cpp
        v1/domain/punycode.cpp
        v1/domain/idna.cpp
        v1/network/ipv4_address.cpp
        v1/network/ipv6_address.cpp
        v1/string/ascii.hpp
        v1/string/join.hpp
        v1/string/locale.hpp
        v1/string/starts_with.hpp
        v1/url/url.cpp
        v1/url/url_search_parameters.cpp

        ${PROJECT_BINARY_DIR}/include/skyr/version.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/config.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/ranges/string_element_range.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/core.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/traits/iterator_value.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/traits/range_iterator.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/traits/range_value.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/views/u8_view.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/views/unchecked_u8_view.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/views/u16_view.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/transforms/byte_transform.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/transforms/u16_transform.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/transforms/u32_transform.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/details/to_bytes.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/domain/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/domain/idna.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/domain/punycode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/domain/domain.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/platform/endianness.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/network/ipv4_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/network/ipv6_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/percent_decode_range.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/percent_encode_range.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/percent_encoded_char.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/percent_decode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/percent_encode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/ranges/string_element_range.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/query/query_parameter_range.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/url_record.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/parse.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/serialize.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/schemes.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/traits/string_traits.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/url_search_parameters.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/url.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/idna.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/punycode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/domain.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/network/ipv4_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/network/ipv6_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_decode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_encode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/query/query_parameter_range.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/url_record.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/parse.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/serialize.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/schemes.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/url.hpp
        )

target_link_libraries(skyr-url PUBLIC tl::expected)
if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang AND ${skyr_BUILD_WITH_LLVM_LIBCXX})
    target_link_libraries(skyr-url PUBLIC "c++")
endif()

set_target_properties(skyr-url PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        POSITION_INDEPENDENT_CODE ON)

target_include_directories(skyr-url
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

if(${CMAKE_CXX_COMPILER_ID} MATCHES MSVC)
    target_compile_definitions(skyr-url -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
endif()

#################################################
# skyr-filesystem
#################################################

add_library(skyr-filesystem INTERFACE)

target_link_libraries(skyr-filesystem INTERFACE skyr-url)
if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    target_link_libraries(skyr-filesystem INTERFACE "stdc++fs")
endif()

target_include_directories(skyr-filesystem
        INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        )

#################################################
# skyr-json
#################################################

if (skyr_ENABLE_JSON_FUNCTIONS)
    add_library(skyr-json INTERFACE)

    target_link_libraries(skyr-json INTERFACE skyr-url nlohmann_json::nlohmann_json)

    target_include_directories(skyr-json
        INTERFACE
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
            )
endif()
