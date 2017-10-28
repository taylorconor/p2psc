#pragma once

#include <cstdint>
#include <p2psc/socket/socket_address.h>

namespace p2psc {

struct PeerIdentifier {
  const socket::SocketAddress socket_address;
  const std::uint8_t version;

  PeerIdentifier(const socket::SocketAddress &socket_address,
                 const std::uint8_t version)
      : socket_address(socket_address), version(version) {}
};
}