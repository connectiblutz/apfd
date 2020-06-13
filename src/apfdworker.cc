#include "apfdworker.h"
#include <iostream>
#include "fileutil.h"
#include "cJSON.h"
#include "logutil.h"
#include "pathutil.h"
#include "stringutil.h"

namespace apfd {

const uint16_t ApfdWorker::MSG_READCONFIG = 1;
const uint16_t ApfdWorker::MSG_CHECKSERVICE = 2;

ApfdWorker::ApfdWorker() {
  configPath=getConfigPath();
  configWatcher = std::make_unique<common::FileWatcher>(configPath,[this]() {
    post(ApfdWorker::MSG_READCONFIG);
  });
  post(ApfdWorker::MSG_READCONFIG);
}
ApfdWorker::~ApfdWorker() {
  servicesList.clear();
}

std::filesystem::path ApfdWorker::getConfigPath() {
  auto configPath = std::filesystem::path("apfd.json");
  if (!std::filesystem::exists(configPath)) {
    configPath = common::PathUtil::binaryPath() / configPath;
  }
  return configPath;
}

void ApfdWorker::OnMessage(common::MessageThread::Message message) {
  common::LogUtil::Debug()<<"received message "<<message.code();
  if (message.code()==ApfdWorker::MSG_READCONFIG) {
    clear(ApfdWorker::MSG_CHECKSERVICE);
    servicesList.clear();
    std::string configStr = common::FileUtil::FileToString(configPath);
    cJSON* config = cJSON_Parse(configStr.c_str());
    cJSON* services = cJSON_GetObjectItem(config,"services");
    if (services && cJSON_IsArray(services)) {
      int size = cJSON_GetArraySize(services);
      for (int i = 0; i < size; i++) {
        cJSON* serviceConfig = cJSON_GetArrayItem(services,i);
        auto service = std::make_shared<ApfdService>(serviceConfig);
        if (service->enabled) {
          servicesList.push_back(service);
          post(Message(ApfdWorker::MSG_CHECKSERVICE,service));
        }
      }
    }
    cJSON_Delete(config);
  } else if (message.code()==ApfdWorker::MSG_CHECKSERVICE) {
    auto service = message.data<ApfdService>();
    bool isOpen = service->isReachable();
    if (!isOpen) {
      if (service->autoStart) {
        service->execStart();
      }
      isOpen = service->isReachable();
    }
    if (isOpen) {
      service->openPort();
    } else {
      service->closePort();
    }
    postDelayed(message,std::chrono::minutes(5));
  }
}


}
