#################################################
# skyr-url
#################################################

add_library(skyr-url-v2 INTERFACE)

#target_sources(
#        skyr-url-v2
#        PRIVATE
#
#        ${PROJECT_BINARY_DIR}/include/skyr/version.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/config.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/errors.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/core.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/concepts/iterator_value.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/concepts/range_iterator.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/concepts/range_value.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/ranges/views/u8_view.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/ranges/views/unchecked_u8_view.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/ranges/views/u16_view.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/ranges/transforms/u8_transform.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/ranges/transforms/u16_transform.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/ranges/transforms/u32_transform.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/ranges/sentinel.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/unicode/details/to_u8.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/string/starts_with.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/containers/static_vector.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/domain/errors.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/domain/idna.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/domain/punycode.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/domain/domain.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/platform/endianness.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/network/ipv4_address.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/network/ipv6_address.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/percent_encoding/errors.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/percent_encoding/sentinel.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/percent_encoding/percent_decode_range.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/percent_encoding/percent_encoded_char.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/percent_encoding/percent_decode.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/percent_encoding/percent_encode.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/core/url_record.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/core/host.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/core/parse.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/core/check_input.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/core/serialize.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/core/schemes.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/core/errors.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/concepts/string_traits.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/url_search_parameters.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/v2/url.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/domain/errors.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/domain/domain.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/network/ipv4_address.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/network/ipv6_address.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_decode.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/percent_encoding/percent_encode.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/core/url_record.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/core/host.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/core/parse.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/core/serialize.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/core/schemes.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/core/errors.hpp
#        ${PROJECT_SOURCE_DIR}/include/skyr/url.hpp
#        )

#set(SKYR_DEBUG_POSTFIX d CACHE STRING "Debug library postfix.")

#set_target_properties(
#        skyr-url-v2
#        PROPERTIES
#        VERSION ${PROJECT_VERSION}
#        SOVERSION ${PROJECT_VERSION_MAJOR}
#        DEBUG_POSTFIX "${SKYR_DEBUG_POSTFIX}"
#)

#target_compile_features(skyr-url-v2 PUBLIC cxx_std_20)

set(full_warnings $<BOOL:${skyr_FULL_WARNINGS}>)
set(warnings_as_errors $<BOOL:${skyr_WARNINGS_AS_ERRORS}>)
set(no_exceptions $<BOOL:${skyr_BUILD_WITHOUT_EXCEPTIONS}>)
set(no_rtti $<BOOL:${skyr_BUILD_WITHOUT_RTTI}>)

set(gnu $<CXX_COMPILER_ID:GNU>)
set(clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>)
set(libcxx $<AND:${clang},$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>)
set(msvc $<CXX_COMPILER_ID:MSVC>)

#target_compile_definitions(
#        skyr-url-v2
#        PRIVATE
#        $<${msvc}:_SCL_SECURE_NO_WARNINGS>
#        $<${msvc}:_CRT_SECURE_NO_DEPRECATE>
#)

#target_compile_options(
#        skyr-url-v2
#        PRIVATE
#        $<$<AND:$<OR:${gnu},${clang}>,${full_warnings}>:-Wall -Wextra>
#        $<$<AND:$<OR:${gnu},${clang}>,${warnings_as_errors}>:-Werror>
#        $<$<AND:$<OR:${gnu},${clang}>,${no_exceptions}>:-fno-exceptions>
#        $<$<AND:$<OR:${gnu},${clang}>,${no_rtti}>:-fno-rtti>
#        $<$<AND:$<OR:${gnu},${clang}>,$<CONFIG:MinSizeRel>>:-flto>
#
#        $<${libcxx}:-stdlib=libc++>
#
#        $<$<AND:${msvc},${full_warnings}>:/W4>
#        $<$<AND:${msvc},${warnings_as_errors}>:/WX>
#        $<$<AND:${msvc},$<NOT:${no_exceptions}>>:/EHsc>
#        $<$<AND:${msvc},${no_rtti}>:/GR->
#        $<${msvc}:/permissive->
#)

target_link_libraries(
        skyr-url-v2
        INTERFACE
        tl::expected
        range-v3
        $<${libcxx}:c++>
)

target_include_directories(
        skyr-url-v2
        INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
#        PRIVATE
#        ${PROJECT_SOURCE_DIR}/src
)

#################################################
# skyr-filesystem
#################################################

if (skyr_ENABLE_FILESYSTEM_FUNCTIONS)
    add_library(skyr-filesystem-v2 INTERFACE)

    target_compile_features(skyr-filesystem-v2 INTERFACE cxx_std_20)

    skyr_check_filesystem(filesystem_definitions)
    target_compile_definitions(
            skyr-filesystem-v2
            INTERFACE
            ${filesystem_definitions}
    )

    target_link_libraries(
            skyr-filesystem-v2
            INTERFACE
            skyr-url-v2
            $<${gnu}:"stdc++fs">
    )

    target_include_directories(
            skyr-filesystem-v2
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
    add_library(skyr-json-v2 INTERFACE)

    target_compile_features(skyr-json-v2 INTERFACE cxx_std_20)

    target_link_libraries(
            skyr-json-v2
            INTERFACE
            skyr-url-v2
            nlohmann_json::nlohmann_json
    )

    target_include_directories(
            skyr-json-v2
            INTERFACE
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    )
endif()
