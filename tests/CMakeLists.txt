function(skyr_create_test file_name output_dir test_name version)
    skyr_remove_extension(${file_name} base)
    set(test ${base}-${version})
    add_executable(${test})
    target_sources(${test} PRIVATE ${base}.cpp)
    target_compile_options(${test} PRIVATE $<${libcxx}:-stdlib=libc++>)
    add_dependencies(${test} skyr-url-${version})
    target_link_libraries(
            ${test}
            PRIVATE
            skyr-url-${version}
            Catch2::Catch2
            Catch2::Catch2WithMain
            fmt::fmt
    )
    set_target_properties(
            ${test}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${output_dir}
    )
    add_test(${test} ${output_dir}/${test})
    set(test_name ${test} PARENT_SCOPE)
endfunction()

if (skyr_BUILD_V1)
    add_subdirectory(v1)
endif()
if (skyr_BUILD_V2)
    add_subdirectory(v2)
endif()
