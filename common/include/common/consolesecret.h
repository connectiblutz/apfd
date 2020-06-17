#pragma once

#include <string>

namespace common {

class ConsoleSecret {
    public:
        static std::string Get(const std::string& prompt);
};

}