cmake_minimum_required(VERSION 3.1.0)

project(ClossANN)

include(cmake/Utilities.cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(OpenANN)
if(NOT OPENANN_FOUND)
    message(FATAL_ERROR "Could not find OpenANN library. You can either "
                        "install it with your package management system or "
                        "download it from http://github.com/OpenANN/OpenANN and "
                        "install it with CMake.")
endif()
find_package(Eigen3)
if(NOT EIGEN3_FOUND)
    message(FATAL_ERROR "Could not find Eigen 3 library. You can either "
                        "install it with your package management system or "
                        "download it from http://eigen.tuxfamily.org/ and "
                        "install it with CMake.")
endif()
include_directories(SYSTEM ${OpenANN_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIRS})
include_directories(src)
set(CLOSS_LINK_LIB ${OpenANN_LIBRARIES})

# Ensure out-of-source build
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Usage: mkdir build; cmake ..")
endif()

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Use C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
set(CLOSS_COMPILER_FLAGS)
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
  message(STATUS "Debug configuration")
  if(CMAKE_COMPILER_IS_GNUCXX)
    compiler_check_add_flag("-ggdb")
    compiler_check_add_flag("-fno-default-inline")
  endif()
elseif(${CMAKE_BUILD_TYPE} STREQUAL Release)
  message(STATUS "Release configuration")
  if(CMAKE_COMPILER_IS_GNUCXX)
    compiler_check_add_flag("-g0")
    compiler_check_add_flag("-s")
    compiler_check_add_flag("-O3")
    compiler_check_add_flag("-msse")
    compiler_check_add_flag("-msse2")
    compiler_check_add_flag("-msse3")
    compiler_check_add_flag("-mssse3")
    compiler_check_add_flag("-msse4.1")
    compiler_check_add_flag("-msse4.2")
  endif()
else()
  message(FATAL_ERROR "Unknown configuration, set CMAKE_BUILD_TYPE to Debug or Release")
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
  set(COMPILER_WARNING_FLAGS "-Wall -Wextra -pedantic -Wno-long-long -Wno-enum-compare")
endif()


add_subdirectory(src)
add_subdirectory(app)

#option(EXCLUDE_TESTS_FROM_ALL "Exclude test suite from standard target." ON)
#if(EXCLUDE_TESTS_FROM_ALL)
#    add_subdirectory(test EXCLUDE_FROM_ALL)
#else()
#    add_subdirectory(test)
#endif()
