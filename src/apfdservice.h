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
    static std::wstring translateIp(const std::wstring& ip);
    static bool isWsl(const std::wstring& ip);
    static std::wstring getWslDistro(const std::wstring& ip);
    static std::wstring getWslInterface(const std::wstring& ip);
  public:
    bool enabled;
    std::wstring name;
    std::wstring protocol;
    std::wstring localIp;
    uint16_t localPort;
    std::wstring remoteIp;
    uint16_t remotePort;
    bool autoStart;
    std::wstring startCommand;
  private:
    bool opened;
};

}