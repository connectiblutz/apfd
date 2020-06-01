#pragma once

#include <string>

namespace apfd {

class WslUtil {
    public:
        static bool isWsl(const std::string& ip);
        static std::string getWslName(const std::string& ip);
        static std::string getWslInterface(const std::string& ip);
        static std::string getWslIp(const std::string& ip);
        static int getWslVersion(const std::string& ip);
};

}