#include "apfdworker.h"
#include <iostream>
#include "fileutil.h"
#include "cJSON.h"
#include "logutil.h"

namespace apfd {

const uint16_t ApfdWorker::MSG_READCONFIG = 1; 

ApfdWorker::ApfdWorker() {
  post(common::MessageThread::Message(MSG_READCONFIG));
}

void ApfdWorker::OnMessage(common::MessageThread::Message message) {
    std::cout << "received message "<<message.code() << std::endl;
    if (message.code()==ApfdWorker::MSG_READCONFIG) {
      std::string configStr = common::FileUtil::FileToString("apfd.json");
      cJSON *config = cJSON_Parse(configStr.c_str());
      LOG(std::cout,cJSON_Print(config));
      cJSON_free(config);
    }
}

}