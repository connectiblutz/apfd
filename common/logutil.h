#pragma once

#include <iostream>
#include "singleton.h"
#include <filesystem>
#include <memory>

namespace apfd::common {

class LogUtil;

class LogLine
{
  public:
    LogLine(std::shared_ptr<LogUtil> logger);
    ~LogLine();
    std::shared_ptr<LogUtil> _logger;
};

template <typename T>
LogLine const & operator << (LogLine const & ll, T arg)
{
    *ll._logger->output() << arg;
    return ll;
}

class LogUtil {
  public:
    static LogLine Debug();
    ~LogUtil() { };
    std::ostream* output() { return _output; }
  protected:
    LogUtil(std::filesystem::path file);
    LogUtil(std::ostream& ostream) : selfManaged(false), _output(&ostream) {};
    friend Singleton::Strong; 
  protected:
    bool selfManaged;
    std::ostream* _output;
};

}