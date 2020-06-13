cmake_minimum_required(VERSION 3.13)
project(apfd)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

if (WIN32)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

add_subdirectory(src)
add_subdirectory(common)
add_subdirectory(cJSON)
if (WIN32)
add_subdirectory(wsl)
endif()
