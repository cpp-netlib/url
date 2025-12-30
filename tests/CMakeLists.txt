function(skyr_create_test file_name output_dir test_name)
    skyr_remove_extension(${file_name} base)
    set(test ${base})
    add_executable(${test})
    target_sources(${test} PRIVATE ${base}.cpp)
    target_compile_options(${test} PRIVATE $<${libcxx}:-stdlib=libc++>)
    add_dependencies(${test} skyr-url)
    target_link_libraries(
            ${test}
            PRIVATE
            skyr-url
            Catch2::Catch2
            Catch2::Catch2WithMain
    )
    set_target_properties(
            ${test}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${output_dir}
    )
    add_test(${test} ${output_dir}/${test})
    set(test_name ${test} PARENT_SCOPE)
endfunction()

add_subdirectory(skyr)
add_subdirectory(wpt)
