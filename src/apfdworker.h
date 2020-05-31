#pragma once

#include "messagethread.h"

namespace apfd {

class ApfdWorker : public common::MessageThread {
  public:
   ApfdWorker();
  protected:
    void OnMessage(common::MessageThread::Message message);
  private:
    static const uint16_t MSG_READCONFIG;
    static const uint16_t MSG_CHECKSERVICE;
    static const std::string POWERSHELL_PREFIX;
    bool isReachable(std::string localIp, uint16_t localPort);
    std::string translateIp(std::string ip);
    bool isWsl(std::string localIp);
    std::string getWslName(std::string localIp);
    std::string getWslIp(std::string localIp);
    void openPort(std::string name, std::string protocol, std::string localIp, uint16_t localPort, std::string remoteIp, uint16_t remotePort);
    void closePort(std::string name, std::string protocol, std::string localIp, uint16_t localPort, std::string remoteIp, uint16_t remotePort);
};

}
