#pragma once

#include <cstdint>
#include <string>

namespace p2psc {
namespace socket {

/**
 * This serves as an abstraction above sockaddr_in. <p2psc/socket.h> will
 * internally convert it to a sockaddr_in.
 */
struct SocketAddress {
  const std::string ip;
  const std::uint16_t port;

  SocketAddress(const std::string &ip, std::uint16_t port)
      : ip(ip), port(port) {}
};
}
}