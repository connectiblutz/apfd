#include "common/socketutil.h"
#include "common/logutil.h"
#include "common/stringutil.h"
#ifdef _WIN32
#include "winsock2.h"
#include "ws2tcpip.h"
#else
#include <arpa/inet.h>
#endif

namespace common {

#ifdef _WIN32
#define inet_pton InetPtonA
Socket::WSAInit::WSAInit() { 
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
}
Socket::WSAInit::~WSAInit() {
  WSACleanup();
}
#else
#include <unistd.h>
#define closesocket close
#endif

Socket::Socket(int af, int type, std::string ip, uint16_t port) : sock(0),connected(false) {
#ifdef _WIN32
  static WSAInit wsaInit;
#endif

  LogUtil::Debug()<<"creating socket to "<<ip<<":"<<port;
  sock = socket(af, type, 0);
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = af; 
  serv_addr.sin_port = htons(port);
  inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);
  if (0==connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) 
  { 
    connected=true;
  }
  
}

Socket::~Socket() {
  if (sock) closesocket(sock);
}

TcpSocket::TcpSocket(std::string ip, uint16_t port) : Socket(AF_INET,SOCK_STREAM, ip, port) {  
}

TcpSocket::~TcpSocket() {

}

std::shared_ptr<Socket> SocketUtil::Create(std::string protocol, std::string ip, uint16_t port) {
  if (protocol=="tcp") return std::dynamic_pointer_cast<Socket>(std::make_shared<TcpSocket>(ip,port));
  return std::shared_ptr<Socket>();
}

}
