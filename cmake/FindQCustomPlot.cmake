# - Try to find QCustomPlot
# Once done, this will define
#
#  QCustomPlot_FOUND - system has QCustomPlot
#  QCustomPlot_INCLUDE_DIRS - the QCustomPlot include directories
#  QCustomPlot_LIBRARIES - link these to use QCustomPlot

include(LibFindMacros)

macro (do_find_package MODULE_NAME)
    # Dependencies
#    libfind_package(${MODULE_NAME})

    # Use pkg-config to get hints about paths
    libfind_pkg_check_modules(${MODULE_NAME}_PKGCONF qcustomplot)

    # Include dir
    find_path(${MODULE_NAME}_INCLUDE_DIR
        NAMES qcustomplot.h
        PATHS ${${MODULE_NAME}_PKGCONF_INCLUDE_DIRS}
    )

    # Finally the library itself
    find_library(${MODULE_NAME}_LIBRARY
        NAMES qcustomplot
        PATHS ${${MODULE_NAME}_PKGCONF_LIBRARY_DIRS}
    )

    # let libfind_process do the rest: export variables, handle REQUIRED, etc.
    # NOTE: libfind_process automatic handle *_INCLUDE_DIR[s] and *_LIBRAR(Y|IES)
    # used by this module and its dependencies
    libfind_process(${MODULE_NAME})
endmacro()

do_find_package(QCustomPlot)
