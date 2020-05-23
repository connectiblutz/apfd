cmake_minimum_required(VERSION 3.8)


add_executable(apfd
  main.cc
)

target_link_libraries(apfd
  cJSON
)

if (WIN32)
  target_link_libraries(apfd
    libcmt.lib
  )
endif()
