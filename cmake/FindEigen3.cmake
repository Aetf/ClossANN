# - Try to find Eigen3
# Once done, this will define
#
#  Eigen3_FOUND - system has Eigen3
#  Eigen3_INCLUDE_DIRS - the Eigen3 include directories
#  Eigen3_LIBRARIES - link these to use Eigen3

include(LibFindMacros)

macro (do_find_package MODULE_NAME)
    # Dependencies
    #libfind_package(${MODULE_NAME} ALGLIB)

    # pkg-config, find headers, find library.
    libfind_pkg_detect(${MODULE_NAME} eigen3
                        FIND_PATH Eigen/Dense)

    # let libfind_process do the rest: export variables, handle REQUIRED, etc.
    # NOTE: libfind_process automatic handle *_INCLUDE_DIR[s] and *_LIBRAR(Y|IES)
    # used by this module and its dependencies
    libfind_process(${MODULE_NAME})
endmacro()

do_find_package(Eigen3)
