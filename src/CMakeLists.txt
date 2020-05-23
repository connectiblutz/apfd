cmake_minimum_required(VERSION 3.8)

add_executable(apfd
  main.cc
  run.cc
  run.h
  install.cc
  install.h
  messagethread.cc
  messagethread.h
)

target_link_libraries(apfd
  cJSON
)
target_compile_options(apfd PRIVATE
 -Werror
 -Wall
 -Wextra
)
