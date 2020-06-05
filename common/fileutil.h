#pragma once

#include <string>
#include <filesystem>

namespace apfd::common {

class FileUtil {
  public:
    static std::wstring FileToString(std::filesystem::path file);
};

}