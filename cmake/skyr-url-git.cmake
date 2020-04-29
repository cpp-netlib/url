# Copyright (c) Glyn Matthews 2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

find_package(Git REQUIRED)

function(skyr_get_git_sha1 output)
    execute_process(COMMAND
            ${GIT_EXECUTABLE} rev-parse -q HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE _output
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(${output} ${_output} PARENT_SCOPE)
endfunction()
