cmake_minimum_required(VERSION 3.1.0)

project(ClossEyeCandy)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)
list(APPEND QT_TARGETS Qt5::Core)
find_package(Qt5Qml)
list(APPEND QT_TARGETS Qt5::Qml)
find_package(Qt5Quick)
list(APPEND QT_TARGETS Qt5::Quick)
find_package(Qt5Widgets)
list(APPEND QT_TARGETS Qt5::Widgets)

find_package(QCustomPlot)
if(QCustomPlot_FOUND)
    find_package(Qt5PrintSupport)
    list(APPEND QT_TARGETS Qt5::PrintSupport)

    add_definitions(-DQCUSTOMPLOT_USE_LIBRARY)
    include_directories(${QCustomPlot_INCLUDE_DIRS})
endif()



# collect all cpp files
aux_source_directory(. SRC_LIST)
aux_source_directory(logic SRC_LIST)
aux_source_directory(models SRC_LIST)
if(QCustomPlot_FOUND)
    aux_source_directory(widgets SRC_LIST)
    list(APPEND SRC_LIST widgets/mainwindow.ui)
endif()
# collect resource file
list(APPEND SRC_LIST res/qml.qrc)
# use QtAwesome
include(libs/UseQtAwesome.cmake)

add_compile_options(${CLOSS_COMPILER_FLAGS})
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} libClossANN)
target_link_libraries(${PROJECT_NAME} ${CLOSS_LINK_LIB})
target_link_libraries(${PROJECT_NAME} ${QT_TARGETS})
if(QCustomPlot_FOUND)
    target_link_libraries(${PROJECT_NAME} ${QCustomPlot_LIBRARIES})
endif()
