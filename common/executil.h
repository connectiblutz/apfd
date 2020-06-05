#pragma once

#include <string>

namespace apfd::common {

class ExecUtil {
  public:
    static std::wstring Run(std::wstring command);
};

}
