#pragma once

#include "messagethread.h"
#include "apfdservice.h"
#include <list>
#include <memory>

namespace apfd {

class ApfdWorker : public common::MessageThread {
  public:
   ApfdWorker();
   ~ApfdWorker();
  protected:
    void OnMessage(common::MessageThread::Message message);
  private:
    static const uint16_t MSG_READCONFIG;
    static const uint16_t MSG_CHECKSERVICE;
    std::list<std::shared_ptr<ApfdService>> servicesList;
};

}
