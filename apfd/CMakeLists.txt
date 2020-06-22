cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)

add_executable(apfd)
target_sources(apfd
  PRIVATE
    src/main.cc
    src/run.cc
    src/install.cc
    src/apfdworker.cc
    src/apfdservice.cc
  PRIVATE
    include/run.h
    include/install.h
    include/apfdworker.h
    include/apfdservice.h
)

target_include_directories(apfd PRIVATE
  include/
)

target_link_libraries(apfd
  bcl
  cJSON
)

if (WIN32)
target_link_libraries(apfd
  wsl
)
endif()