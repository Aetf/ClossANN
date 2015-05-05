# - Try to find ALGLIB
# Once done, this will define
#
#  ALGLIB_FOUND - system has ALGLIB
#  ALGLIB_INCLUDE_DIRS - the ALGLIB include directories
#  ALGLIB_LIBRARIES - link these to use ALGLIB

include(LibFindMacros)

set(MODULE_NAME ALGLIB)

macro (do_find_package MODULE_NAME)
    # Dependencies
    #libfind_package(${MODULE_NAME} ALGLIB)

    # Use pkg-config to get hints about paths
    libfind_pkg_check_modules(${MODULE_NAME}_PKGCONF alglib)

    # Include dir
    find_path(${MODULE_NAME}_INCLUDE_DIR
        NAMES optimization.h
        PATHS
            ${${MODULE_NAME}_PKGCONF_INCLUDE_DIRS}
            /usr/include/libalglib
    )

    # Finally the library itself
    find_library(${MODULE_NAME}_LIBRARY
        NAMES alglib
        PATHS ${${MODULE_NAME}_PKGCONF_LIBRARY_DIRS}
    )

    # let libfind_process do the rest: export variables, handle REQUIRED, etc.
    # NOTE: libfind_process automatic handle *_INCLUDE_DIR[s] and *_LIBRAR(Y|IES)
    # used by this module and its dependencies
    libfind_process(${MODULE_NAME})
endmacro()

do_find_package(ALGLIB)
