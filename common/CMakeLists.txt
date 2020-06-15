cmake_minimum_required(VERSION 3.13)

add_library(common)
target_sources(common
  PRIVATE
    src/messagethread.cc
    src/consolehandler.cc
    src/fileutil.cc
    src/logutil.cc
    src/executil.cc
    src/socketutil.cc
    src/stringutil.cc
    src/singleton.cc
    src/pathutil.cc
    src/firewallcontrol.cc
    src/filewatcher.cc
    src/servicecontrol.cc
  PUBLIC
    include/common/messagethread.h
    include/common/consolehandler.h
    include/common/fileutil.h
    include/common/logutil.h
    include/common/priorityqueueex.h
    include/common/executil.h
    include/common/socketutil.h
    include/common/stringutil.h
    include/common/singleton.h
    include/common/pathutil.h
    include/common/firewallcontrol.h
    include/common/filewatcher.h
    include/common/servicecontrol.h
)

target_include_directories(common PUBLIC
  include/
)

if (WIN32)
target_sources(common
  PRIVATE
    src/windowsservice.cc
  PUBLIC
    include/common/windowsservice.h
)
target_link_libraries(common PUBLIC
  Ws2_32 # socketutil
  ole32 # firewallcontrol
  oleaut32 # firewallcontrol
)
set_target_properties(common PROPERTIES COMPILE_DEFINITIONS
  _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)
target_compile_definitions(common PRIVATE
  UNICODE
)
endif()