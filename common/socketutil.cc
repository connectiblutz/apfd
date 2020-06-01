#include "socketutil.h"
#include "logutil.h"
#include "winsock2.h"
#include "ws2tcpip.h"

namespace apfd::common {

Socket::WSAInit::WSAInit() { 
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
}
Socket::WSAInit::~WSAInit() {
  WSACleanup();
}

Socket::Socket(int af, int type, std::string ip, uint16_t port) : sock(0),connected(false) {
  static WSAInit wsaInit;

  LogUtil::Debug()<<"creating socket to "<<ip<<":"<<port;
  sock = socket(af, type, 0);
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = af; 
  serv_addr.sin_port = htons(port); 
  InetPtonA(AF_INET, ip.c_str(), &serv_addr.sin_addr);
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
