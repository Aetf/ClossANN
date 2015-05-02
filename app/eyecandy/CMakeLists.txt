cmake_minimum_required(VERSION 3.1.0)

project(ClossEyeCandy)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)
find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5Widgets)

list(APPEND QT_TARGETS Qt5::Core)
list(APPEND QT_TARGETS Qt5::Qml)
list(APPEND QT_TARGETS Qt5::Quick)
list(APPEND QT_TARGETS Qt5::Widgets)

# collect all cpp files
aux_source_directory(. SRC_LIST)
aux_source_directory(logic SRC_LIST)
aux_source_directory(models SRC_LIST)
# collect resource file
list(APPEND SRC_LIST res/qml.qrc)

add_definitions(${CLOSS_COMPILER_FLAGS})
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} libClossANN)
target_link_libraries(${PROJECT_NAME} ${CLOSS_LINK_LIB})
target_link_libraries(${PROJECT_NAME} ${QT_TARGETS})
