#include "apfdservice.h"
#include "executil.h"
#include "socketutil.h"
#include "stringutil.h"
#include "wslutil.h"
#include "firewallutil.h"

namespace apfd {

const std::wstring ApfdService::POWERSHELL_PREFIX=L"powershell -ExecutionPolicy Bypass -Command ";

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

  if (name && cJSON_IsString(name)) this->name = common::StringUtil::toWide(cJSON_GetStringValue(name));
  if (protocol && cJSON_IsString(protocol)) this->protocol = common::StringUtil::toWide(cJSON_GetStringValue(protocol));
  if (localIp && cJSON_IsString(localIp)) this->localIp = common::StringUtil::toWide(cJSON_GetStringValue(localIp));
  if (localPort && cJSON_IsNumber(localPort)) this->localPort = cJSON_GetNumberValue(localPort);
  if (remoteIp && cJSON_IsString(remoteIp)) this->remoteIp = common::StringUtil::toWide(cJSON_GetStringValue(remoteIp));
  if (remotePort && cJSON_IsNumber(remotePort)) this->remotePort = cJSON_GetNumberValue(remotePort);

  
  cJSON* autoStart = cJSON_GetObjectItem(config,"autoStart");
  cJSON* startCommand = cJSON_GetObjectItem(config,"startCommand");
  if (autoStart && cJSON_IsBool(autoStart)) this->autoStart=cJSON_IsTrue(autoStart);
  if (startCommand && cJSON_IsString(startCommand)) this->startCommand = common::StringUtil::toWide(cJSON_GetStringValue(startCommand));
}

ApfdService::~ApfdService() {
  if (opened) closePort();
}

std::wstring ApfdService::translateIp(const std::wstring& ip) {
  if (ip==L"any") return L"0.0.0.0";
  if (ip==L"localhost") return L"127.0.0.1";
  if (ApfdService::isWsl(ip)) {
    auto distro = ApfdService::getWslDistro(ip);
    auto intf = ApfdService::getWslInterface(ip);
    return wsl::WslUtil::getIP(distro,intf);
  }
  return ip;
}

bool ApfdService::isReachable() {
  auto socket = common::SocketUtil::Create(protocol,ApfdService::translateIp(localIp),localPort);
  if (socket) return socket->isConnected();
  return false;
}

void ApfdService::openPort() {
  opened=true;
  common::FirewallUtil::Open(name,common::FirewallUtil::Direction::ANY,protocol,localIp,localPort,remoteIp,remotePort);
  common::ExecUtil::Run(ApfdService::POWERSHELL_PREFIX+L"New-NetFireWallRule -DisplayName 'APFD "+name+L"' -Direction Outbound -LocalPort "+std::to_wstring(remotePort)+L" -Action Allow -Protocol "+protocol);
  common::ExecUtil::Run(ApfdService::POWERSHELL_PREFIX+L"New-NetFireWallRule -DisplayName 'APFD "+name+L"' -Direction Inbound -LocalPort "+std::to_wstring(remotePort)+L" -Action Allow -Protocol "+protocol);  
  common::ExecUtil::Run(L"netsh interface portproxy add v4tov4 listenport="+std::to_wstring(remotePort)+L" listenaddress="+ApfdService::translateIp(remoteIp)+L" connectport="+std::to_wstring(localPort)+L" connectaddress="+ApfdService::translateIp(localIp)+L" protocol="+protocol);
}

void ApfdService::closePort() {
  common::FirewallUtil::Close(name,common::FirewallUtil::Direction::ANY,protocol,localIp,localPort,remoteIp,remotePort);
  common::ExecUtil::Run(ApfdService::POWERSHELL_PREFIX+L"Remove-NetFireWallRule -DisplayName 'APFD "+name+L"' ");  
  common::ExecUtil::Run(L"netsh interface portproxy delete v4tov4 listenport="+std::to_wstring(remotePort)+L" listenaddress="+ApfdService::translateIp(remoteIp)+L" protocol="+protocol);
  opened=false;
}

void ApfdService::execStart() {  
  if (ApfdService::isWsl(localIp)) {
    std::wstring vmName = ApfdService::getWslDistro(localIp);
    wsl::WslUtil::run(vmName,startCommand);
  } else {
    common::ExecUtil::Run(startCommand);
  }
}


bool ApfdService::isWsl(const std::wstring& ip) {
  return (ip.size()>1 && ip[0]==L':' && ip[1]!=L':');
}

std::wstring ApfdService::getWslDistro(const std::wstring& ip) {
  if (ApfdService::isWsl(ip)) {
    auto parts = common::StringUtil::split(ip,':');
    if (parts.size()>=2) return parts[1];
  }
  return L"";
}
std::wstring ApfdService::getWslInterface(const std::wstring& ip) {
  if (ApfdService::isWsl(ip)) {
    auto distro = ApfdService::getWslDistro(ip);
    auto parts = common::StringUtil::split(ip,L':');
    if (parts.size()>=3) return parts[2];
    auto version = wsl::WslUtil::getVersion(distro);
    switch (version) {
      case 2:
        return L"eth0";
      case 1:
      default:
        return L"lo";
    }
  }
  return L"";
}

}