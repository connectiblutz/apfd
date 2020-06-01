#include "logutil.h"
#include <fstream>
#include <ctime>

namespace apfd::common {

LogUtil::LogUtil(std::filesystem::path file) : selfManaged(true) {  
  _output = new std::ofstream(file.string());
}

LogLine LogUtil::Debug() {
  auto logger = Singleton::get<LogUtil>();
  if (!logger) {
    logger = Singleton::Strong::create<LogUtil>(std::cout);
  }
  auto ll = LogLine(logger);
  return ll;
}


LogLine::LogLine(std::shared_ptr<LogUtil> logger) : _logger (logger) {
  time_t now = time(0);
  struct tm p;
  localtime_s(&p,&now);
  char str[20];
  strftime(str, sizeof str, "%F-%T", &p);
  *_logger->output() << str << " : ";
}

LogLine::~LogLine () { 
  *_logger->output() << std::endl;
}

}