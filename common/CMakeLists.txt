cmake_minimum_required(VERSION 3.13)

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
  stringutil.cc
  stringutil.h
  singleton.cc
  singleton.h
  pathutil.cc
  pathutil.h
  firewallcontrol.cc
  firewallcontrol.h
  filewatcher.cc
  filewatcher.h
  servicecontrol.cc
  servicecontrol.h
)

target_include_directories(common PUBLIC
  .
)

if (WIN32)
target_link_libraries(common PUBLIC
  Ws2_32
)
set_target_properties(common PROPERTIES COMPILE_DEFINITIONS
  _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)
target_compile_definitions(common PRIVATE
  UNICODE
)
endif()