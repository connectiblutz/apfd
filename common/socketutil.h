#pragma once

#include <string>
#include <memory>

namespace apfd::common {

class Socket {
  public:
    Socket(int af, int type, std::string ip, uint16_t port);
    virtual ~Socket();
    bool isConnected() const { return connected; }
  protected:
    int sock;
    bool connected;
  private:
    class WSAInit {
      public:
        WSAInit();
        ~WSAInit();   
    };
};

class TcpSocket : public Socket {
  public:
    TcpSocket(std::string ip, uint16_t port);
    ~TcpSocket();
};

class SocketUtil {
  public:
    static std::shared_ptr<Socket> Create(std::string protcol, std::string ip, uint16_t port);
};

}