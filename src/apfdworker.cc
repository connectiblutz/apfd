#include "apfdworker.h"
#include <iostream>
#include "fileutil.h"
#include "cJSON.h"
#include "logutil.h"
#include "executil.h"

#define UNUSED(x) (void)(x)

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
    cJSON* services = cJSON_GetObjectItem(config,"services");
    if (services && cJSON_IsArray(services)) {
      int size = cJSON_GetArraySize(services);
      for (int i = 0; i < size; i++) {
        cJSON* service = cJSON_GetArrayItem(services,i);
        std::shared_ptr<cJSON> servicePtr(cJSON_Duplicate(service,true), [](auto p) {
           cJSON_Delete(p);
        });
        post(Message(ApfdWorker::MSG_CHECKSERVICE,servicePtr));
      }
    }
    cJSON_Delete(config);
  } else if (message.code()==ApfdWorker::MSG_CHECKSERVICE) {
    cJSON* service = message.data<cJSON>().get();

    cJSON* enabled = cJSON_GetObjectItem(service,"enabled");
    if (enabled && cJSON_IsBool(enabled) && cJSON_IsFalse(enabled)) return;

    cJSON* localIp = cJSON_GetObjectItem(service,"localIp");
    cJSON* localPort = cJSON_GetObjectItem(service,"localPort");
    cJSON* remoteIp = cJSON_GetObjectItem(service,"remoteIp");
    cJSON* remotePort = cJSON_GetObjectItem(service,"remotePort");
    if (!localIp || !cJSON_IsString(localIp)) return;
    if (!localPort || !cJSON_IsNumber(localPort)) return;
    if (!remoteIp || !cJSON_IsString(remoteIp)) return;
    if (!remotePort || !cJSON_IsNumber(remotePort)) return;
    std::string localIpStr = cJSON_GetStringValue(localIp);
    uint16_t localPortNum = cJSON_GetNumberValue(localPort);
    std::string remoteIpStr = cJSON_GetStringValue(remoteIp);
    uint16_t remotePortNum = cJSON_GetNumberValue(remotePort);

    bool isOpen = isReachable(localIpStr,localPortNum);
    if (!isOpen) {
      cJSON* startCommand = cJSON_GetObjectItem(service,"startCommand");
      if (startCommand && cJSON_IsString(startCommand)) {
        std::string startCommandStr = cJSON_GetStringValue(startCommand);
        if (isWSL(localIpStr)) {
          std::string vmName = getWSLName(localIpStr);
          common::ExecUtil::Run("wsl -d "+vmName+" -- "+startCommandStr);
        } else {
          common::ExecUtil::Run(startCommandStr);
        }
      }
      isOpen = isReachable(localIpStr,localPortNum);
    }
    if (isOpen) {
      openPort(localIpStr,localPortNum,remoteIpStr,remotePortNum);
    } else {
      closePort(localIpStr,localPortNum,remoteIpStr,remotePortNum);
    }

    postDelayed(Message(ApfdWorker::MSG_CHECKSERVICE,message.data<cJSON>()),std::chrono::milliseconds(1*60*1000));
  }
}

bool ApfdWorker::isReachable(std::string localIp, uint16_t localPort) {
  UNUSED(localIp);
  UNUSED(localPort);
  return false;
}
bool ApfdWorker::isWSL(std::string localIp) {
  return (localIp.size()>1 && localIp[0]==':' && localIp[1]!=':');
}

std::string ApfdWorker::getWSLName(std::string localIp) {
  if (isWSL(localIp)) return localIp.substr(1);
  return "";
}

void ApfdWorker::openPort(std::string localIp, uint16_t localPort, std::string remoteIp, uint16_t remotePort) {
  UNUSED(localIp);
  UNUSED(localPort);
  UNUSED(remoteIp);
  UNUSED(remotePort);
}

void ApfdWorker::closePort(std::string localIp, uint16_t localPort, std::string remoteIp, uint16_t remotePort) {
  UNUSED(localIp);
  UNUSED(localPort);
  UNUSED(remoteIp);
  UNUSED(remotePort);
}

}
