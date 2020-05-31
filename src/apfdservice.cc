#include "apfdservice.h"
#include "executil.h"

namespace apfd {

const std::string ApfdService::POWERSHELL_PREFIX="powershell -ExecutionPolicy Bypass -Command ";

ApfdService::ApfdService(cJSON* config) : enabled(false), autoStart(false) {
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
  closePort();
}

std::string ApfdService::translateIp(std::string ip) {
  if (ip=="any") return "0.0.0.0";
  if (ip=="localhost") return "127.0.0.1";
  if (ApfdService::isWsl(ip)) return ApfdService::getWslIp(ip);
  return ip;
}

bool ApfdService::isReachable() {
  return false;
}

bool ApfdService::isWsl(std::string ip) {
  return (ip.size()>1 && ip[0]==':' && ip[1]!=':');
}

std::string ApfdService::getWslName(std::string ip) {
  if (isWsl(ip)) return ip.substr(1);
  return "";
}

std::string ApfdService::getWslIp(std::string ip) {
  if (!ApfdService::isWsl(ip)) return ip;
  std::string vmName = ApfdService::getWslName(ip);
  std::string output = common::ExecUtil::Run("wsl -d "+vmName+" -- "+" ip -4 addr show eth0 | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'");
  return output;
}

void ApfdService::openPort() {
  common::ExecUtil::Run(ApfdService::POWERSHELL_PREFIX+"New-NetFireWallRule -DisplayName 'APFD "+name+"' -Direction Outbound -LocalPort $ports_a -Action Allow -Protocol "+protocol);
  common::ExecUtil::Run(ApfdService::POWERSHELL_PREFIX+"New-NetFireWallRule -DisplayName 'APFD "+name+"' -Direction Inbound -LocalPort $ports_a -Action Allow -Protocol "+protocol);  
  common::ExecUtil::Run("netsh interface portproxy add v4tov4 listenport="+std::to_string(remotePort)+" listenaddress="+remoteIp+" connectport="+std::to_string(localPort)+" connectaddress="+localIp+" protocol="+protocol);
}

void ApfdService::closePort() {
  common::ExecUtil::Run(ApfdService::POWERSHELL_PREFIX+"Remove-NetFireWallRule -DisplayName 'APFD "+name+"' ");  
  common::ExecUtil::Run("netsh interface portproxy delete v4tov4 listenport="+std::to_string(remotePort)+" listenaddress="+remoteIp+" protocol="+protocol);
}

void ApfdService::execStart() {  
  if (ApfdService::isWsl(localIp)) {
    std::string vmName = ApfdService::getWslName(localIp);
    common::ExecUtil::Run("wsl -d "+vmName+" -- "+startCommand);
  } else {
    common::ExecUtil::Run(startCommand);
  }
}

}