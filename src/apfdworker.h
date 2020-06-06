#pragma once

#include "messagethread.h"
#include "apfdservice.h"
#include <list>
#include <memory>
#include "filewatcher.h"
#include <filesystem>

namespace apfd {

class ApfdWorker : public common::MessageThread {
  public:
   ApfdWorker();
   ~ApfdWorker();
  protected:
    void OnMessage(common::MessageThread::Message message);
    std::filesystem::path getConfigPath();
  private:
    static const uint16_t MSG_READCONFIG;
    static const uint16_t MSG_CHECKSERVICE;
    std::list<std::shared_ptr<ApfdService>> servicesList;
    std::unique_ptr<common::FileWatcher> configWatcher;
    std::filesystem::path configPath;
};

}
