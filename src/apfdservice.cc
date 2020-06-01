#include "apfdservice.h"
#include "executil.h"
#include "socketutil.h"
#include "stringutil.h"
#include "wslutil.h"

namespace apfd {

const std::string ApfdService::POWERSHELL_PREFIX="powershell -ExecutionPolicy Bypass -Command ";

    bool opened;
ApfdService::ApfdService(cJSON* config) : enabled(false), autoStart(false), opened(false) {
  cJSON* enabled = cJSON_GetObjectItem(config,"enabled");
  if (enabled && cJSON_IsBool(enabled)) this->enabled=cJSON_IsTrue(enabled);

  cJSON* name = cJSON_GetObjectItem(config,"name");
  cJSON* protocol = cJSON_GetObjectItem(config,"protocol");
  cJSON* localIp = cJSON_GetObjectItem(config,"localIp");
  cJSON* localPort = cJSON_GetObjectItem(config,"localPort");
  cJSON* remoteIp = cJSON_GetObjectItem(config,"remoteIp");
  cJSON* remotePort = cJSON_GetObjectItem(config,"remotePort");

  if (name && cJSON_IsString(name)) this->name = cJSON_GetStringValue(name);
  if (protocol && cJSON_IsString(protocol)) this->protocol = cJSON_GetStringValue(protocol);
  if (localIp && cJSON_IsString(localIp)) this->localIp = cJSON_GetStringValue(localIp);
  if (localPort && cJSON_IsNumber(localPort)) this->localPort = cJSON_GetNumberValue(localPort);
  if (remoteIp && cJSON_IsString(remoteIp)) this->remoteIp = cJSON_GetStringValue(remoteIp);
  if (remotePort && cJSON_IsNumber(remotePort)) this->remotePort = cJSON_GetNumberValue(remotePort);

  
  cJSON* autoStart = cJSON_GetObjectItem(config,"autoStart");
  cJSON* startCommand = cJSON_GetObjectItem(config,"startCommand");
  if (autoStart && cJSON_IsBool(autoStart)) this->autoStart=cJSON_IsTrue(autoStart);
  if (startCommand && cJSON_IsString(startCommand)) this->startCommand = cJSON_GetStringValue(startCommand);
}

ApfdService::~ApfdService() {
  if (opened) closePort();
}

std::string ApfdService::translateIp(const std::string& ip) {
  if (ip=="any") return "0.0.0.0";
  if (ip=="localhost") return "127.0.0.1";
  if (WslUtil::isWsl(ip)) return WslUtil::getWslIp(ip);
  return ip;
}

bool ApfdService::isReachable() {
  auto socket = common::SocketUtil::Create(protocol,ApfdService::translateIp(localIp),localPort);
  if (socket) return socket->isConnected();
  return false;
}

void ApfdService::openPort() {
  opened=true;
  common::ExecUtil::Run(ApfdService::POWERSHELL_PREFIX+"New-NetFireWallRule -DisplayName 'APFD "+name+"' -Direction Outbound -LocalPort "+std::to_string(remotePort)+" -Action Allow -Protocol "+protocol);
  common::ExecUtil::Run(ApfdService::POWERSHELL_PREFIX+"New-NetFireWallRule -DisplayName 'APFD "+name+"' -Direction Inbound -LocalPort "+std::to_string(remotePort)+" -Action Allow -Protocol "+protocol);  
  common::ExecUtil::Run("netsh interface portproxy add v4tov4 listenport="+std::to_string(remotePort)+" listenaddress="+ApfdService::translateIp(remoteIp)+" connectport="+std::to_string(localPort)+" connectaddress="+ApfdService::translateIp(localIp)+" protocol="+protocol);
}

void ApfdService::closePort() {
  common::ExecUtil::Run(ApfdService::POWERSHELL_PREFIX+"Remove-NetFireWallRule -DisplayName 'APFD "+name+"' ");  
  common::ExecUtil::Run("netsh interface portproxy delete v4tov4 listenport="+std::to_string(remotePort)+" listenaddress="+ApfdService::translateIp(remoteIp)+" protocol="+protocol);
  opened=false;
}

void ApfdService::execStart() {  
  if (WslUtil::isWsl(localIp)) {
    std::string vmName = WslUtil::getWslName(localIp);
    common::ExecUtil::Run("wsl -d "+vmName+" -- "+startCommand);
  } else {
    common::ExecUtil::Run(startCommand);
  }
}

}