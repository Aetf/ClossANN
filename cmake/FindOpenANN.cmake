find_path(OPENANN_INCLUDE_DIRS OpenANN
    ${CMAKE_INSTALL_PREFIX}/include/OpenANN
    /usr/include/OpenANN
    /opt/local/include/OpenANN
    DOC "OpenANN include directory")

set(OPENANN_FOUND ${OPENANN_INCLUDE_DIRS} CACHE BOOL "" FORCE)
