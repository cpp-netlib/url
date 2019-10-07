# Copyright (c) Glyn Matthews 2018-19.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


set(
        EXAMPLES
        example_01
        example_02
        example_03
        example_04
        example_05
        example_06
        example_07
        example_08
        example_09
    )

foreach(example ${EXAMPLES})
        add_executable(${example} ${example}.cpp)
        add_dependencies(${example} skyr-url)
        target_link_libraries(${example} PRIVATE skyr-url)
        set_target_properties(${example} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Skyr_BINARY_DIR}/examples)
endforeach (example)
