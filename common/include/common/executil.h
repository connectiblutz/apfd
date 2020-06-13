#pragma once

#include <string>

namespace common {

class ExecUtil {
  public:
    static std::string Run(std::string command);
};

}
