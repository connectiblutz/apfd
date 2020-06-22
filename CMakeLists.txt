cmake_minimum_required(VERSION 3.13)
project(apfd VERSION 0.5.0 LANGUAGES CXX C)

add_subdirectory(apfd)
add_subdirectory(bcl)
add_subdirectory(third_party/cJSON)
if (WIN32)
add_subdirectory(wsl)
endif()
