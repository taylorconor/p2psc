#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <p2psc/socket/socket_address.h>
#include <p2psc/socket/socket_exception.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

namespace p2psc {
namespace socket {
const int RECV_BUF_SIZE = 1024;
}

class Socket {
public:
  Socket(const socket::SocketAddress &socket_address);
  Socket(int sock_fd);
  ~Socket();

  void send(const std::string &);
  std::string receive();
  socket::SocketAddress get_socket_address();
  void close();

private:
  Socket(const Socket &) = delete;

  void _connect();
  void _check_is_open();

  int _sock_fd;
  bool _is_open;
  struct sockaddr_in _address;
};
}