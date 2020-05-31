cmake_minimum_required(VERSION 3.8)

add_library(common
  messagethread.cc
  messagethread.h
  windowsservice.cc
  windowsservice.h
  consolehandler.cc
  consolehandler.h
  fileutil.cc
  fileutil.h
  logutil.cc
  logutil.h
  priorityqueueex.h
  executil.cc
  executil.h
  socketutil.cc
  socketutil.h
)

target_include_directories(common PUBLIC
  .
)

if (WIN32)
target_link_libraries(common PUBLIC
  Ws2_32
)
endif()