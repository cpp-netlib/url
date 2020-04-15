# Copyright (c) Glyn Matthews 2018-20.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Doxygen REQUIRED)
find_package(Pandoc REQUIRED)
find_package(Sphinx REQUIRED)

# Copy RST files
foreach(doc_file
        _build
        _static
        _templates
        index.rst
        url.rst
        core.rst
        domain.rst
        network.rst
        percent_encoding.rst
        filesystem.rst
        json.rst)
    file(COPY ${doc_file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# Generate API documentation using Doxygen
configure_file(
        Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

configure_file(
        conf.py.in
        ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)

add_custom_target(
        doxygen ALL
        COMMAND
            ${DOXYGEN}
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Generating documentation with Doxygen"
)

# Convert markdown to RST
SET(
        GENERATED_FROM_MARKDOWN
        changelog.rst
)

foreach (file_name ${GENERATED_FROM_MARKDOWN})
    skyr_remove_extension(${file_name} basename)
    string(TOUPPER ${basename} basename_upper)
    set(target ${basename})
    string(PREPEND target "doc_")
    add_custom_target(
            ${target}
            pandoc ${PROJECT_SOURCE_DIR}/${basename_upper}.md -f markdown -t rst -s -o ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
            WORKING_DIRECTORY
                ${CMAKE_CURRENT_BINARY_DIR}
            BYPRODUCTS
                ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
            COMMENT
                "Generating RST file with pandoc" VERBATIM
    )
endforeach()

# Bring it all together using Sphinx
add_custom_target(
        doc ALL
        COMMAND
            ${SPHINX_EXECUTABLE} -M html ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
            doxygen ${GENERATED_FROM_MARKDOWN}
        COMMENT
            "Generating documentation with Sphinx"
)
