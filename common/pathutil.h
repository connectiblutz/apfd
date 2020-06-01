#pragma once

#include <filesystem>


namespace apfd::common {

class PathUtil {
  public:
    static std::filesystem::path binaryExe();
    static std::filesystem::path binaryPath();
    static void chdir(std::filesystem::path path);
};

}