cmake_minimum_required(VERSION 2.8.12)

project(libClossANN)

aux_source_directory(. SRC_LIST)

add_definitions(${CLOSS_COMPILER_FLAGS})
add_library(${PROJECT_NAME} STATIC ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${CLOSS_LINK_LIB})
