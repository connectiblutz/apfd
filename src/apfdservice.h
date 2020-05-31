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
    static std::string translateIp(std::string ip);
    static bool isWsl(std::string ip);
    static std::string getWslName(std::string ip);
    static std::string getWslIp(std::string ip);
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
    static const std::string POWERSHELL_PREFIX;
};

}