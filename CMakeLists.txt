cmake_minimum_required(VERSION 3.8)
project(apfd)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

if (WIN32)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)
add_subdirectory(common)
add_subdirectory(cJSON)
