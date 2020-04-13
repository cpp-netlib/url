# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

configure_file(
        version.hpp.in
        ${PROJECT_SOURCE_DIR}/include/skyr/version.hpp
)

add_library(
        skyr-url

        core/url_parser_context.hpp
        core/url_parser_context.cpp
        core/url_parse.cpp
        core/url_parse_impl.hpp
        core/url_serialize.cpp
        domain/domain.cpp
        domain/punycode.cpp
        domain/idna.cpp
        network/ipv4_address.cpp
        network/ipv6_address.cpp
        string/ascii.hpp
        string/join.hpp
        string/locale.hpp
        string/split.hpp
        string/starts_with.hpp
        url/url.cpp
        url/url_search_parameters.cpp
        filesystem/path.cpp

        ${PROJECT_SOURCE_DIR}/include/skyr/config.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/version.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/url_record.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/url_parse.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/url_serialize.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/url_schemes.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/url_error.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/traits/string_traits.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/core.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/traits/iterator_value.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/traits/range_iterator.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/traits/range_value.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/ranges/views/u8_view.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/ranges/views/unchecked_u8_view.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/ranges/views/u16_view.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/ranges/transforms/byte_transform.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/ranges/transforms/u16_transform.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/ranges/transforms/u32_transform.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/unicode/details/to_bytes.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/idna.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/punycode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/domain.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_decode_range.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_encode_range.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_encoded_char.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_decode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_encode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/network/ipv4_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/network/ipv6_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/query/query_iterator.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/platform/endianness.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/url/url_record.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/url/url_parse.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/url/url_serialize.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/url/url_error.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/url_search_parameters.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/filesystem/path.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/url.hpp)

target_link_libraries(skyr-url PUBLIC tl::expected)
if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang AND ${skyr_BUILD_WITH_LLVM_LIBCXX})
    target_link_libraries(skyr-url PUBLIC "c++")
endif()
if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    target_link_libraries(skyr-url PUBLIC "stdc++fs")
endif()

set_target_properties(skyr-url PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        POSITION_INDEPENDENT_CODE ON)

target_include_directories(skyr-url
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

add_library(skyr-json INTERFACE)

target_link_libraries(skyr-json INTERFACE tl::expected nlohmann_json::nlohmann_json)

target_include_directories(skyr-json
        INTERFACE
        ${PROJECT_SOURCE_DIR}/include)
