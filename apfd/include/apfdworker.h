#pragma once

#include "common/messagethreadpool.h"
#include "apfdservice.h"
#include <list>
#include <memory>
#include "common/filewatcher.h"
#include <filesystem>

namespace apfd {

class ApfdWorker : public common::MessageThreadPool {
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
