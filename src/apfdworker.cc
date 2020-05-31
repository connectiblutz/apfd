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

const std::string ApfdWorker::POWERSHELL_PREFIX="powershell -ExecutionPolicy Bypass -Command ";

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

    cJSON* name = cJSON_GetObjectItem(service,"name");
    cJSON* protocol = cJSON_GetObjectItem(service,"protocol");
    cJSON* localIp = cJSON_GetObjectItem(service,"localIp");
    cJSON* localPort = cJSON_GetObjectItem(service,"localPort");
    cJSON* remoteIp = cJSON_GetObjectItem(service,"remoteIp");
    cJSON* remotePort = cJSON_GetObjectItem(service,"remotePort");
    if (!name || !cJSON_IsString(name)) return;
    if (!protocol || !cJSON_IsString(protocol)) return;
    if (!localIp || !cJSON_IsString(localIp)) return;
    if (!localPort || !cJSON_IsNumber(localPort)) return;
    if (!remoteIp || !cJSON_IsString(remoteIp)) return;
    if (!remotePort || !cJSON_IsNumber(remotePort)) return;
    std::string nameStr = cJSON_GetStringValue(name);
    std::string protocolStr = cJSON_GetStringValue(protocol);
    std::string localIpStr = cJSON_GetStringValue(localIp);
    uint16_t localPortNum = cJSON_GetNumberValue(localPort);
    std::string remoteIpStr = cJSON_GetStringValue(remoteIp);
    uint16_t remotePortNum = cJSON_GetNumberValue(remotePort);

    bool isOpen = isReachable(localIpStr,localPortNum);
    if (!isOpen) {
      cJSON* startCommand = cJSON_GetObjectItem(service,"startCommand");
      if (startCommand && cJSON_IsString(startCommand)) {
        std::string startCommandStr = cJSON_GetStringValue(startCommand);
        if (isWsl(localIpStr)) {
          std::string vmName = getWslName(localIpStr);
          common::ExecUtil::Run("wsl -d "+vmName+" -- "+startCommandStr);
        } else {
          common::ExecUtil::Run(startCommandStr);
        }
      }
      isOpen = isReachable(translateIp(localIpStr),localPortNum);
    }
    if (isOpen) {
      openPort(nameStr,protocolStr,translateIp(localIpStr),localPortNum,translateIp(remoteIpStr),remotePortNum);
    } else {
      closePort(nameStr,protocolStr,translateIp(localIpStr),localPortNum,translateIp(remoteIpStr),remotePortNum);
    }

    postDelayed(message,std::chrono::seconds(15));
  }
}

std::string ApfdWorker::translateIp(std::string ip) {
  if (ip=="any") return "0.0.0.0";
  if (ip=="localhost") return "127.0.0.1";
  if (isWsl(ip)) return getWslIp(ip);
  return ip;
}

bool ApfdWorker::isReachable(std::string localIp, uint16_t localPort) {
  UNUSED(localIp);
  UNUSED(localPort);
  return false;
}

bool ApfdWorker::isWsl(std::string localIp) {
  return (localIp.size()>1 && localIp[0]==':' && localIp[1]!=':');
}

std::string ApfdWorker::getWslName(std::string localIp) {
  if (isWsl(localIp)) return localIp.substr(1);
  return "";
}

std::string ApfdWorker::getWslIp(std::string localIp) {
  if (!isWsl(localIp)) return localIp;
  std::string vmName = getWslName(localIp);
  std::string output = common::ExecUtil::Run("wsl -d "+vmName+" -- "+" ip -4 addr show eth0 | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'");
  return output;
}

void ApfdWorker::openPort(std::string name, std::string protocol, std::string localIp, uint16_t localPort, std::string remoteIp, uint16_t remotePort) {
  closePort(name,protocol,localIp,localPort,remoteIp,remotePort);
  common::ExecUtil::Run(ApfdWorker::POWERSHELL_PREFIX+"New-NetFireWallRule -DisplayName 'APFD "+name+"' -Direction Outbound -LocalPort $ports_a -Action Allow -Protocol "+protocol);
  common::ExecUtil::Run(ApfdWorker::POWERSHELL_PREFIX+"New-NetFireWallRule -DisplayName 'APFD "+name+"' -Direction Inbound -LocalPort $ports_a -Action Allow -Protocol "+protocol);  
  common::ExecUtil::Run("netsh interface portproxy add v4tov4 listenport="+std::to_string(remotePort)+" listenaddress="+remoteIp+" connectport="+std::to_string(localPort)+" connectaddress="+localIp+" protocol="+protocol);
}

void ApfdWorker::closePort(std::string name, std::string protocol, std::string localIp, uint16_t localPort, std::string remoteIp, uint16_t remotePort) {
  UNUSED(localIp);
  UNUSED(localPort);
  common::ExecUtil::Run(ApfdWorker::POWERSHELL_PREFIX+"Remove-NetFireWallRule -DisplayName 'APFD "+name+"' ");  
  common::ExecUtil::Run("netsh interface portproxy delete v4tov4 listenport="+std::to_string(remotePort)+" listenaddress="+remoteIp+" protocol="+protocol);
}

}
