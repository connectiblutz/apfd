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
    bool isReachable(std::string localIp, uint16_t localPort);
    bool isWSL(std::string localIp);
    std::string getWSLName(std::string localIp);
    void openPort(std::string localIp, uint16_t localPort, std::string remoteIp, uint16_t remotePort);
    void closePort(std::string localIp, uint16_t localPort, std::string remoteIp, uint16_t remotePort);
};

}
