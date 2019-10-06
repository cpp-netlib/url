#Look for an executable called pandoc
find_program(PANDOC_EXECUTABLE
        NAMES pandoc
        DOC "Path to pandoc executable")

include(FindPackageHandleStandardArgs)

#Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(Pandoc
        "Failed to find pandoc-build executable"
        PANDOC_EXECUTABLE)
