#pragma once

#include <ctime>
#include <iostream>

namespace apfd::common {

#define LOG(out,output) \
  {\
  time_t now = time(0);\
  struct tm p;\
  localtime_s(&p,&now);\
  char str[20];\
  strftime(str, sizeof str, "%F-%T", &p);\
  out << str << " : ";\
  out << output;\
  out << std::endl;\
  }

}