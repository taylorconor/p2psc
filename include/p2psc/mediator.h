#pragma once

#include <p2psc/socket/socket_address.h>

namespace p2psc {

struct Mediator {
  const socket::SocketAddress socket_address;

  Mediator(const std::string &ip, std::uint16_t port)
      : socket_address(ip, port) {}
};
}