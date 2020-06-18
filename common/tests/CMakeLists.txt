cmake_minimum_required(VERSION 3.13)

add_executable(common_test)
target_sources(common_test
  PRIVATE
    messagethreadpool_perf.cc
)

target_link_libraries(common_test
  common
)