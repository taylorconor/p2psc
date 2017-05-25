#pragma once

#include <p2psc/peer.h>
#include <p2psc/socket/socket_address.h>

namespace p2psc {

struct PunchedPeer {
  const Peer peer;
  const socket::SocketAddress address;

  PunchedPeer(const Peer &peer, const socket::SocketAddress address)
      : peer(peer), address(address) {}
};
}