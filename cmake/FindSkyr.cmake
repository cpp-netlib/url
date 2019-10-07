find_path(SKYR_INCLUDE_DIR skyr/url.hpp
        PATH_SUFFIXES skyr)

find_library(SKYR_LIBRARY NAME Skyr::skyr-url)


include(FindPackageHandleStandardArgs)

#Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(Skyr
        DEFAULT_MSG
        SKYR_INCLUDE_DIR SKYR_LIBRARY_NAME)
mark_as_advanced(SKYR_INCLUDE_DIR SKYR_LIBRARY_NAMES)


set(SKYR_LIBRARIES ${SKYR_LIBRARY} )
set(SKYR_INCLUDE_DIRS ${SKYR_INCLUDE_DIR} )
