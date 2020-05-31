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
};

}
