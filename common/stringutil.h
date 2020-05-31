#pragma once

#include <string>
#include <vector>

namespace apfd::common {

class StringUtil {
  public:
    static std::string reverse(const std::string& s);
    static std::string ltrim(const std::string& s);
    static std::string rtrim(const std::string& s);
    static std::string trim(const std::string& s);
    static std::string asciiToHex(const std::string& s);
    static std::vector<std::string> split(const std::string& s, const char token);
};
}