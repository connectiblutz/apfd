#pragma once
#include "cJSON.h"
#include <string>

namespace apfd {

class ApfdService {
  public:
    ApfdService(cJSON* config);
    ~ApfdService();
    bool isReachable();
    void execStart();
    void openPort();
    void closePort();
  private:
    static std::string translateIp(const std::string& ip);
    static bool isWsl(const std::string& ip);
    static std::string getWslDistro(const std::string& ip);
    static std::string getWslInterface(const std::string& ip);
  public:
    bool enabled;
    std::string name;
    std::string protocol;
    std::string localIp;
    uint16_t localPort;
    std::string remoteIp;
    uint16_t remotePort;
    bool autoStart;
    std::string startCommand;
  private:
    bool opened;
};

}