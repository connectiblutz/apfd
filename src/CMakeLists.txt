cmake_minimum_required(VERSION 3.13)

add_executable(apfd
  main.cc
  run.cc
  run.h
  install.cc
  install.h
  apfdworker.cc
  apfdworker.h
  apfdservice.cc
  apfdservice.h
)

target_link_libraries(apfd
  common
  cJSON
)

if (WIN32)
target_link_libraries(apfd
  wsl
)
endif()