#pragma once

#include <boost/optional.hpp>
#include <p2psc/socket/socket.h>
#include <p2psc/socket/socket_address.h>
#include <p2psc/socket_creator.h>
#include <string>

namespace p2psc {
namespace socket {

class LocalListeningSocket {
public:
  LocalListeningSocket(SocketCreator socket_creator);
  LocalListeningSocket(SocketCreator socket_creator, uint16_t port);
  ~LocalListeningSocket();

  std::shared_ptr<Socket> accept() const;
  void close();

  socket::SocketAddress get_socket_address() const;

private:
  LocalListeningSocket(const LocalListeningSocket &) = delete;

  int _sockfd;
  uint16_t _port;
  bool _is_open;
  SocketCreator _socket_creator;
};
}
}