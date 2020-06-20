#pragma once

#include <bcl/messagethreadpool.h>
#include "apfdservice.h"
#include <list>
#include <memory>
#include <bcl/filewatcher.h>
#include <filesystem>

namespace apfd {

class ApfdWorker : public bcl::MessageThreadPool {
  public:
   ApfdWorker();
   ~ApfdWorker();
  protected:
    void OnMessage(bcl::MessageThread::Message message);
    std::filesystem::path getConfigPath();
  private:
    static const uint16_t MSG_READCONFIG;
    static const uint16_t MSG_CHECKSERVICE;
    std::list<std::shared_ptr<ApfdService>> servicesList;
    std::unique_ptr<bcl::FileWatcher> configWatcher;
    std::filesystem::path configPath;
};

}
