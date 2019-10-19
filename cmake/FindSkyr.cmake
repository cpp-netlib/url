# https://gitlab.kitware.com/cmake/community/wikis/doc/tutorials/How-To-Find-Libraries

find_package(PkgConfig)
pkg_check_modules(PC_Skyr QUIET skyr-url)

find_path(Skyr_INCLUDE_DIR
        NAMES skyr/url.hpp
        HINTS ${Skyr_INCLUDE_DIR_HINT} ${PC_Skyr_INCLUDEDIR} ${PC_Skyr_INCLUDE_DIRS}
        )

find_library(Skyr_LIBRARY
        NAME skyr-url
        HINTS ${Skyr_LIB_DIR_HINT} ${PC_Skyr_LIBDIR} ${PC_Skyr_LIBRARY_DIRS}
        )

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Skyr
        DEFAULT_MSG
        Skyr_INCLUDE_DIR Skyr_LIBRARY)

if (Skyr_FOUND)
    mark_as_advanced(Skyr_INCLUDE_DIR Skyr_LIBRARY)
    set(Skyr_LIBRARIES ${Skyr_LIBRARY})
    set(Skyr_INCLUDE_DIRS ${Skyr_INCLUDE_DIR})
endif()
