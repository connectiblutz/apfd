#pragma once

#include <string>
#include <functional>
#include <memory>
#include <windows.h>
#include <netfw.h>

namespace apfd::common {
  class FirewallControl {
    public:
      enum Direction {
        INBOUND, OUTBOUND, ANY
      };
      FirewallControl(const std::wstring& name,Direction direction, const std::wstring& protocol, const std::wstring& remoteIp, uint16_t remotePort);
      void open();
      void close();
    private:
      void commonSetup(std::function<void(std::shared_ptr<INetFwRules>)> cb);
      void open(Direction direction);
      void close(Direction direction);
      std::wstring _name;
      Direction _direction;
      std::wstring _protocol;
      std::wstring _remoteIp;
      uint16_t _remotePort;
    
  };
}