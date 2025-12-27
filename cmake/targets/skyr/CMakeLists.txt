#################################################
# skyr-url (C++23)
# Header-only library - no compiled code
#################################################

add_library(skyr-url INTERFACE)

target_compile_features(skyr-url INTERFACE cxx_std_23)

target_link_libraries(
        skyr-url
        INTERFACE
        uni-algo::uni-algo
        $<${libcxx}:c++>
)

target_include_directories(
        skyr-url
        INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
)

# Add aliases for compatibility
add_library(skyr::skyr-url ALIAS skyr-url)
add_library(skyr::url ALIAS skyr-url)

#################################################
# skyr-filesystem
#################################################

if (skyr_ENABLE_FILESYSTEM_FUNCTIONS)
    add_library(skyr-filesystem INTERFACE)

    target_compile_features(skyr-filesystem INTERFACE cxx_std_23)

    skyr_check_filesystem(filesystem_definitions)
    target_compile_definitions(
            skyr-filesystem
            INTERFACE
            ${filesystem_definitions}
    )

    target_link_libraries(
            skyr-filesystem
            INTERFACE
            skyr-url
    )

    target_include_directories(
            skyr-filesystem
            INTERFACE
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
            )

    add_library(skyr::skyr-filesystem ALIAS skyr-filesystem)
    add_library(skyr::filesystem ALIAS skyr-filesystem)
endif()

#################################################
# skyr-json
#################################################

if (skyr_ENABLE_JSON_FUNCTIONS)
    add_library(skyr-json INTERFACE)

    target_compile_features(skyr-json INTERFACE cxx_std_23)

    target_link_libraries(
            skyr-json
            INTERFACE
            skyr-url
            nlohmann_json::nlohmann_json
    )

    target_include_directories(
            skyr-json
            INTERFACE
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    )

    add_library(skyr::skyr-json ALIAS skyr-json)
    add_library(skyr::json ALIAS skyr-json)
endif()
