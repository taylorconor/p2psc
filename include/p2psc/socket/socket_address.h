#pragma once

#include <cstdint>
#include <string>

namespace p2psc {
namespace socket {

/**
 * This serves as an abstraction above sockaddr_in. <p2psc/socket.h> will
 * internally convert it to a sockaddr_in.
 */
class SocketAddress {
public:
  SocketAddress(const std::string &ip, std::uint16_t port)
      : _ip(ip), _port(port) {}

  std::string ip() const { return _ip; }
  std::uint16_t port() const { return _port; }

private:
  std::string _ip;
  std::uint16_t _port;
};
}
}