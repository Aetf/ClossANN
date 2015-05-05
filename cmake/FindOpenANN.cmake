# - Try to find OpenANN
# Once done, this will define
#
#  OpenANN_FOUND - system has OpenANN
#  OpenANN_INCLUDE_DIRS - the OpenANN include directories
#  OpenANN_LIBRARIES - link these to use OpenANN

include(LibFindMacros)

macro (do_find_package MODULE_NAME)
    # Dependencies
    libfind_package(${MODULE_NAME} ALGLIB)
    libfind_package(${MODULE_NAME} Eigen3)

    # pkg-config, find headers, find library.
    libfind_pkg_detect(${MODULE_NAME} openann
                        FIND_PATH OpenANN
                        FIND_LIBRARY openann)

    # let libfind_process do the rest: export variables, handle REQUIRED, etc.
    # NOTE: libfind_process automatic handle *_INCLUDE_DIR[s] and *_LIBRAR(Y|IES)
    # used by this module and its dependencies
    libfind_process(${MODULE_NAME})
endmacro()

do_find_package(OpenANN)
