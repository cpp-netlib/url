#################################################
# skyr-url
#################################################

add_library(skyr-url-v2 INTERFACE)

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
            $<${clang_with_gnu_stdlib}:"stdc++fs">
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
