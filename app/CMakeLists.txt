cmake_minimum_required(VERSION 3.1.0)

project(ApplyClossANN)

aux_source_directory(. SRC_LIST)

add_definitions(${CLOSS_COMPILER_FLAGS})
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${CLOSS_LINK_LIB})
