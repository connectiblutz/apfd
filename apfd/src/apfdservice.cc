#include "apfdservice.h"
#include <bcl/executil.h>
#include <bcl/socketutil.h>
#include <bcl/stringutil.h>
#include <bcl/firewallcontrol.h>
#ifdef _WIN32
#include <bcl/servicecontrol.h>
#include "wsl/wslutil.h"
#endif

namespace apfd {

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

#ifdef _WIN32
  if (this->enabled && ApfdService::isWsl(this->localIp)) {
    auto service = bcl::ServiceControl("vmcompute");
    service.start();
  }
#endif
}

ApfdService::~ApfdService() {
  if (opened) closePort();
}

std::string ApfdService::translateIp(const std::string& ip) {
  if (ip=="any") return "0.0.0.0";
  if (ip=="localhost") return "127.0.0.1";
#ifdef _WIN32
  if (ApfdService::isWsl(ip)) {
    auto distro = ApfdService::getWslDistro(ip);
    auto intf = ApfdService::getWslInterface(ip);
    return wsl::WslUtil::getIP(distro,intf);
  }
#endif
  return ip;
}

bool ApfdService::isReachable() {
  auto socket = bcl::SocketUtil::Create(protocol,ApfdService::translateIp(localIp),localPort);
  if (socket) return socket->isConnected();
  return false;
}

void ApfdService::openPort() {
  closePort();
  opened=true;
  auto fc = bcl::FirewallControl(name,bcl::FirewallControl::Direction::ANY,protocol,remoteIp,remotePort);
  fc.open();
#ifdef _WIN32
  bcl::ExecUtil::Run("netsh interface portproxy add v4tov4 listenport="+std::to_string(remotePort)+" listenaddress="+ApfdService::translateIp(remoteIp)+" connectport="+std::to_string(localPort)+" connectaddress="+ApfdService::translateIp(localIp)+" protocol="+protocol);
#endif
}

void ApfdService::closePort() {
  auto fc = bcl::FirewallControl(name,bcl::FirewallControl::Direction::ANY,protocol,remoteIp,remotePort);
  fc.close();
#ifdef _WIN32
  bcl::ExecUtil::Run("netsh interface portproxy delete v4tov4 listenport="+std::to_string(remotePort)+" listenaddress="+ApfdService::translateIp(remoteIp)+" protocol="+protocol);
#endif
  opened=false;
}

void ApfdService::execStart() {  
#ifdef _WIN32
  if (ApfdService::isWsl(localIp)) {
    std::string vmName = ApfdService::getWslDistro(localIp);
    wsl::WslUtil::run(vmName,startCommand);
  } else {
#endif
    bcl::ExecUtil::Run(startCommand);
#ifdef _WIN32
  }
#endif
}


#ifdef _WIN32
bool ApfdService::isWsl(const std::string& ip) {
  return (ip.size()>1 && ip[0]==L':' && ip[1]!=L':');
}

std::string ApfdService::getWslDistro(const std::string& ip) {
  if (ApfdService::isWsl(ip)) {
    auto parts = bcl::StringUtil::split(ip,':');
    if (parts.size()>=2) return parts[1];
  }
  return "";
}
std::string ApfdService::getWslInterface(const std::string& ip) {
  if (ApfdService::isWsl(ip)) {
    auto distro = ApfdService::getWslDistro(ip);
    auto parts = bcl::StringUtil::split(ip,L':');
    if (parts.size()>=3) return parts[2];
    auto version = wsl::WslUtil::getVersion(distro);
    switch (version) {
      case 2:
        return "eth0";
      case 1:
      default:
        return "lo";
    }
  }
  return "";
}
#endif

}