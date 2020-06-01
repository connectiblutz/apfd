cmake_minimum_required(VERSION 3.8)

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
  wslutil.cc
  wslutil.h
)

target_link_libraries(apfd
  common
  cJSON
)
target_compile_options(apfd PRIVATE
 -Werror
 -Wall
 -Wextra
)
