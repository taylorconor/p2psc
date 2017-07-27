#pragma once

#include <p2psc/socket/socket_address.h>

namespace p2psc {

struct Mediator {
  const socket::SocketAddress socket_address;

  Mediator(const socket::SocketAddress &socket_address)
      : socket_address(socket_address) {}
};
}