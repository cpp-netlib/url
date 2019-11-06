# Copyright (c) Glyn Matthews 2018-19.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Doxygen REQUIRED)
find_package(Pandoc REQUIRED)
find_package(Sphinx REQUIRED)

set(DOC_FILES
        _build
        _static
        _templates
        index.rst
        unicode.rst
        idna.rst
        domain.rst
        encoding.rst
        concepts.rst
        url.rst
        url_record.rst
        url_error_codes.rst
        url_search_parameters.rst
        )

# Copy RST files
foreach(doc_file ${DOC_FILES})
    file(COPY ${doc_file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# Generate API documentation using Doxygen
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in
        ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)

add_custom_target(doxygen ALL
        COMMAND
        ${DOXYGEN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen")

# Convert markdown to RST
SET(
        GENERATED_FROM_MARKDOWN
        readme.rst
        changelog.rst
)

foreach (file_name ${GENERATED_FROM_MARKDOWN})
    _remove_extension(${file_name} basename)
    string(TOUPPER ${basename} basename_upper)
    add_custom_target(${file_name}
            pandoc ${CMAKE_SOURCE_DIR}/${basename_upper}.md -f markdown -t rst -s -o ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
            COMMENT "Generating RST file with pandoc" VERBATIM)
endforeach()

# Bring it all together using Sphinx
add_custom_target(doc ALL
        COMMAND
        ${SPHINX_EXECUTABLE} -M html ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS doxygen ${GENERATED_FROM_MARKDOWN}
        COMMENT "Generating documentation with Sphinx")
