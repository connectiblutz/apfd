#pragma once

#include <string>
#include <functional>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#include <netfw.h>
#endif

namespace common {
  class FirewallControl {
    public:
      enum Direction {
        INBOUND, OUTBOUND, ANY
      };
      FirewallControl(const std::string& name,Direction direction, const std::string& protocol, const std::string& remoteIp, uint16_t remotePort);
      void open();
      void close();
    private:
#ifdef _WIN32
      void commonSetup(std::function<void(std::shared_ptr<INetFwRules>)> cb);
#endif
      void open(Direction direction);
      void close(Direction direction);
      std::string _name;
      Direction _direction;
      std::string _protocol;
      std::string _remoteIp;
      uint16_t _remotePort;
    
  };
}