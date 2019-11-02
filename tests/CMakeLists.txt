# Copyright (c) Glyn Matthews 2012-2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


find_package(Catch2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

if (MSVC)
    # For json.hpp
    add_definitions(-D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING)
endif(MSVC)

function(_create_test file_name output_dir test_name)
    _remove_extension(${file_name} test)
    add_executable(${test} ${test}.cpp)
    add_dependencies(${test} Skyr::skyr-url)
    target_link_libraries(
            ${test} PRIVATE ${CMAKE_THREAD_LIBS_INIT} Skyr::skyr-url Catch2::Catch2 nlohmann_json nlohmann_json::nlohmann_json)
    set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${output_dir})
    set(${test_name} ${test} PARENT_SCOPE)
endfunction()

add_subdirectory(unicode)
add_subdirectory(url)
