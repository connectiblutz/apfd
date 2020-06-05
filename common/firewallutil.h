#pragma once

#include <string>

namespace apfd::common {
  class FirewallUtil {
    public:
      enum Direction {
        INBOUND, OUTBOUND, ANY
      };
      static void Open(const std::wstring& name,Direction direction, const std::wstring& protocol, const std::wstring& localIp, uint16_t localPort, const std::wstring& remoteIp, uint16_t remotePort);
      static void Close(const std::wstring& name,Direction direction, const std::wstring& protocol, const std::wstring& localIp, uint16_t localPort, const std::wstring& remoteIp, uint16_t remotePort);
    
  };
}