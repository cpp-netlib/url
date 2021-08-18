#################################################
# skyr-url
#################################################

add_library(skyr-url-v1 STATIC)

target_sources(
        skyr-url-v1
        PRIVATE

        core/url_parser_context.hpp
        core/url_parser_context.cpp
        core/parse.cpp
        core/url_parse_impl.hpp
        network/ipv4_address.cpp
        network/ipv4_address.cpp
        network/ipv6_address.cpp
        domain/idna.cpp
        url/url.cpp
        url/url_search_parameters.cpp

        ${PROJECT_BINARY_DIR}/include/skyr/version.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/config.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/core.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/traits/iterator_value.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/traits/range_iterator.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/traits/range_value.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/views/u8_view.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/views/unchecked_u8_view.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/views/u16_view.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/transforms/u8_transform.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/transforms/u16_transform.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/transforms/u32_transform.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/ranges/sentinel.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/unicode/details/to_u8.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/string/starts_with.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/containers/static_vector.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/domain/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/domain/idna.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/domain/punycode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/domain/domain.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/platform/endianness.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/network/ipv4_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/network/ipv6_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/sentinel.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/percent_decode_range.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/percent_encoded_char.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/percent_decode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/percent_encoding/percent_encode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/url_record.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/host.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/parse.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/check_input.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/serialize.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/schemes.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/core/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/traits/string_traits.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/url_search_parameters.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/v1/url.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/domain/domain.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/network/ipv4_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/network/ipv6_address.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_decode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_encode.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/url_record.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/host.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/parse.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/serialize.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/schemes.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/core/errors.hpp
        ${PROJECT_SOURCE_DIR}/include/skyr/url.hpp
        )

set(SKYR_DEBUG_POSTFIX d CACHE STRING "Debug library postfix.")

set_target_properties(
        skyr-url-v1
        PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        DEBUG_POSTFIX "${SKYR_DEBUG_POSTFIX}"
)

target_compile_features(skyr-url-v1 PUBLIC cxx_std_17)

target_compile_definitions(
        skyr-url-v1
        PRIVATE
        $<${msvc}:_SCL_SECURE_NO_WARNINGS>
        $<${msvc}:_CRT_SECURE_NO_DEPRECATE>
)

target_compile_options(
        skyr-url-v1
        PRIVATE
        $<$<AND:$<OR:${gnu},${clang}>,${full_warnings}>:-Wall -Wextra>
        $<$<AND:$<OR:${gnu},${clang}>,${warnings_as_errors}>:-Werror>
        $<$<AND:$<OR:${gnu},${clang}>,${no_exceptions}>:-fno-exceptions>
        $<$<AND:$<OR:${gnu},${clang}>,${no_rtti}>:-fno-rtti>
        $<$<AND:$<OR:${gnu},${clang}>,$<CONFIG:MinSizeRel>>:-flto>

        $<${libcxx}:-stdlib=libc++>

        $<$<AND:${msvc},${full_warnings}>:/W4>
        $<$<AND:${msvc},${warnings_as_errors}>:/WX>
        $<$<AND:${msvc},$<NOT:${no_exceptions}>>:/EHsc>
        $<$<AND:${msvc},${no_rtti}>:/GR->
        $<${msvc}:/permissive->
)

target_link_libraries(
        skyr-url-v1
        PUBLIC
        tl::expected
        range-v3
        PRIVATE
        $<${libcxx}:c++>
)

target_include_directories(
        skyr-url-v1
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

#################################################
# skyr-filesystem
#################################################

if (skyr_ENABLE_FILESYSTEM_FUNCTIONS)
    add_library(skyr-filesystem-v1 INTERFACE)

    target_compile_features(skyr-filesystem-v1 INTERFACE cxx_std_17)

    skyr_check_filesystem(filesystem_definitions)
    target_compile_definitions(
            skyr-filesystem-v1
            INTERFACE
            ${filesystem_definitions}
    )

    target_link_libraries(
            skyr-filesystem-v1
            INTERFACE
            skyr-url-v1
            $<${gnu}:"stdc++fs">
            $<${clang_with_gnu_stdlib}:"stdc++fs">
    )

    target_include_directories(
            skyr-filesystem-v1
            INTERFACE
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
            )
endif()

#################################################
# skyr-json
#################################################

if (skyr_ENABLE_JSON_FUNCTIONS)
    add_library(skyr-json-v1 INTERFACE)

    target_compile_features(skyr-json-v1 INTERFACE cxx_std_17)

    target_link_libraries(
            skyr-json-v1
            INTERFACE
            skyr-url-v1
            nlohmann_json::nlohmann_json
    )

    target_include_directories(
            skyr-json-v1
            INTERFACE
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    )
endif()
