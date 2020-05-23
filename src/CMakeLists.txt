cmake_minimum_required(VERSION 3.8)

add_executable(apfd
  main.cc
  run.cc
  run.h
  install.cc
  install.h
)

target_link_libraries(apfd
  cJSON
)
target_compile_options(apfd PRIVATE
 -Werror
 -Wall
 -Weverything
 -Wno-reserved-id-macro
)

if (WIN32)
  target_link_libraries(apfd
    libcmt.lib
  )
endif()
