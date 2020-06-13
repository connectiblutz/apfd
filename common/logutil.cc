#include "logutil.h"
#include <fstream>
#include <ctime>

namespace apfd::common {

LogUtil::LogUtil(std::filesystem::path file) : selfManaged(true) {  
  _output = new std::ofstream(file.wstring());
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
  _lock=std::unique_lock<std::mutex>(logger->_lineMutex);
  time_t now = time(0);
  struct tm p;
  localtime_s(&p,&now);
  char str[20];
  strftime(str, sizeof str, "%F-%T", &p);
  *_logger->output() << str << " : ";
}

LogLine::LogLine(LogLine&& other) {
  _logger.swap(other._logger);
  _lock.swap(other._lock);
}

LogLine::~LogLine () { 
  if (_logger) *_logger->output() << std::endl;
  if (_lock) _lock.unlock();
}

}