#pragma once

#include <boost/optional.hpp>
#include <p2psc/socket.h>
#include <p2psc/socket/socket_address.h>
#include <string>

namespace p2psc {
namespace integration {
namespace util {

class LocalListeningSocket {
public:
  LocalListeningSocket();
  ~LocalListeningSocket();

  std::shared_ptr<Socket> accept();
  void close();

  socket::SocketAddress get_socket_address();

private:
  int _sockfd;
  uint16_t _port;
};
}
}
}