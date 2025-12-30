# Web Platform Tests (WPT)
# These are NOT part of the regular test suite - they generate a report only

if (NOT skyr_BUILD_WPT)
    return()
endif()

# Require nlohmann-json for parsing WPT test data
find_package(nlohmann_json REQUIRED)

# Create the WPT test runner executable
add_executable(wpt_runner)
target_sources(wpt_runner PRIVATE wpt_runner.cpp)

target_compile_options(wpt_runner PRIVATE $<${libcxx}:-stdlib=libc++>)

target_link_libraries(
    wpt_runner
    PRIVATE
    skyr-url
    nlohmann_json::nlohmann_json
)

# Set C++23 and enable parallel execution support
target_compile_features(wpt_runner PRIVATE cxx_std_23)

# Link with TBB for parallel execution policy support (required on some platforms)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # For GCC and Clang, we may need TBB for parallel algorithms
    find_package(TBB QUIET)
    if (TBB_FOUND)
        target_link_libraries(wpt_runner PRIVATE TBB::tbb)
        message(STATUS "WPT runner: Using TBB for parallel execution")
    else()
        message(STATUS "WPT runner: TBB not found, using built-in parallel execution support")
    endif()
endif()

set_target_properties(
    wpt_runner
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests/wpt
)

# Note: We do NOT add this to CTest (add_test) because it's a reporting tool,
# not a pass/fail test suite.

message(STATUS "Web Platform Tests enabled")
message(STATUS "  Run with: ${PROJECT_BINARY_DIR}/tests/wpt/wpt_runner")
message(STATUS "  Options: --force-download (fetch latest data)")