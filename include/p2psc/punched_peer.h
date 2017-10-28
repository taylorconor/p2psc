#pragma once

#include <p2psc/peer.h>
#include <p2psc/socket/socket_address.h>

namespace p2psc {

struct PunchedPeer {
  const Peer peer;
  const socket::SocketAddress address;
  const std::uint8_t version;

  PunchedPeer(const Peer &peer, const socket::SocketAddress address,
              const std::uint8_t version)
      : peer(peer), address(address), version(version) {}
  PunchedPeer &operator=(const PunchedPeer &) { return *this; }
};
}