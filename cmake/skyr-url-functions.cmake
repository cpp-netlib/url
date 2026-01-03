# Copyright (c) Glyn Matthews 2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


include(CheckCXXSourceCompiles)


function(skyr_replace_dynamic_msvcrt_linker_flags)
    # Replace dynamic MSVCRT linker flags with static version.
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
endfunction()

function(skyr_remove_extension file_name basename)
    string(REGEX REPLACE "\\.[^.]*$" "" _basename ${file_name})
    set(${basename} ${_basename} PARENT_SCOPE)
endfunction()


# Legacy filesystem check function removed
# C++23 guarantees std::filesystem support, so no checks are needed
