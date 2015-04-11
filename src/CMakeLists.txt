cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SRC_LIST)

add_definitions(${MY_COMPILER_FLAGS})
add_executable(bpann ${SRC_LIST})
target_link_libraries(bpann openann)
