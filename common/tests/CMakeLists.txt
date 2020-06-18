cmake_minimum_required(VERSION 3.13)

add_executable(messagethreadpool_perf
  messagethreadpool_perf.cc
)
target_link_libraries(messagethreadpool_perf
  common
)

add_executable(threadpool_class_perf
  threadpool_class_perf.cc
)
target_link_libraries(threadpool_class_perf
  common
)

add_executable(threadpool_lambda_perf
  threadpool_lambda_perf.cc
)
target_link_libraries(threadpool_lambda_perf
  common
)