#pragma once

#include <string>

namespace apfd::common {

class ExecUtil {
  public:
    static std::string Run(std::string command);
};

}
