#include "apfdworker.h"
#include <iostream>
#include "fileutil.h"
#include "cJSON.h"
#include "logutil.h"

namespace apfd {

const uint16_t ApfdWorker::MSG_READCONFIG = 1;
const uint16_t ApfdWorker::MSG_CHECKSERVICE = 2;

ApfdWorker::ApfdWorker() {
  post(ApfdWorker::MSG_READCONFIG);
}

void ApfdWorker::OnMessage(common::MessageThread::Message message) {
  LOG(std::cout,"received message "<<message.code());
  if (message.code()==ApfdWorker::MSG_READCONFIG) {
    clear(ApfdWorker::MSG_CHECKSERVICE);
    std::string configStr = common::FileUtil::FileToString("apfd.json");
    cJSON* config = cJSON_Parse(configStr.c_str());
    if (cJSON_HasObjectItem(config,"services")) {
      cJSON* services = cJSON_GetObjectItem(config,"services");
      if (cJSON_IsArray(services)) {
        int size = cJSON_GetArraySize(services);
        for (int i = 0; i < size; i++) {
          cJSON* service = cJSON_GetArrayItem(services,i);
          std::shared_ptr<cJSON> servicePtr(cJSON_Duplicate(service,true), [](auto p) {
             cJSON_Delete(p);
          });
          post(Message(ApfdWorker::MSG_CHECKSERVICE,servicePtr));
        }
      }
    }
    cJSON_Delete(config);
  } else if (message.code()==ApfdWorker::MSG_CHECKSERVICE) {
    cJSON* service = message.data<cJSON>().get();
    if (cJSON_HasObjectItem(service,"localIp")) {
      
    }
  }
}

}
