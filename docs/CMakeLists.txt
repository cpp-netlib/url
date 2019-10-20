# Copyright (c) Glyn Matthews 2018-19.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Doxygen REQUIRED)
find_package(Pandoc REQUIRED)
find_package(Sphinx REQUIRED)

# Copy RST files
file(COPY _build DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY _static DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY _templates DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY index.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY url.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY url_record.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY url_search_parameters.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY url_error_codes.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY unicode.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

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
        readme
        changelog
)

foreach (basename ${GENERATED_FROM_MARKDOWN})
    string(TOUPPER ${basename} basename_upper)
    add_custom_target(${basename}
            pandoc ${CMAKE_SOURCE_DIR}/${basename_upper}.md -f markdown -t rst -s -o ${CMAKE_CURRENT_BINARY_DIR}/${basename}.rst
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            BYPRODUCTS $CMAKE_CURRENT_BINARY_DIR}/${basename}.rst
            COMMENT "Generating RST file with pandoc" VERBATIM)
endforeach()

# Bring it all together using Sphinx
add_custom_target(doc ALL
        COMMAND
        ${SPHINX_EXECUTABLE} -M html ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS doxygen ${GENERATED_FROM_MARKDOWN}
        COMMENT "Generating documentation with Sphinx")
