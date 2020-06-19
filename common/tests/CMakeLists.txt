cmake_minimum_required(VERSION 3.13)

enable_testing()

find_package(GTest CONFIG REQUIRED)
# target_link_libraries(<name> PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
# add_test(AllTestsInMain <name>)

add_executable(messagethreadpool_perf
  src/messagethreadpool_perf.cc
)
target_link_libraries(messagethreadpool_perf
  common
)

add_executable(threadpool_class_perf
  src/threadpool_class_perf.cc
)
target_link_libraries(threadpool_class_perf
  common
)

add_executable(threadpool_lambda_perf
  src/threadpool_lambda_perf.cc
)
target_link_libraries(threadpool_lambda_perf
  common
)