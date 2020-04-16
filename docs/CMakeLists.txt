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

configure_file(
        Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

configure_file(
        conf.py.in
        ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)

add_custom_target(
        doc ALL
        COMMAND
            ${DOXYGEN}
        COMMAND
            ${PANDOC_EXECUTABLE} ${PROJECT_SOURCE_DIR}/CHANGELOG.md -f markdown -t rst -s -o ${CMAKE_CURRENT_BINARY_DIR}/changelog.rst
        COMMAND
            ${SPHINX_EXECUTABLE} -M html ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND
            touch ${CMAKE_CURRENT_BINARY_DIR}/html/.nojekyll
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Generating documentation with Doxygen and Sphinx"
)
