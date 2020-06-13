#pragma once

#include <string>
#include <filesystem>

namespace common {

class FileUtil {
  public:
    static std::string FileToString(std::filesystem::path file);
};

}