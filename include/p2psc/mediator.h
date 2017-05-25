#pragma once

#include <p2psc/key/keypair.h>
#include <p2psc/peer.h>
#include <p2psc/socket.h>
#include <p2psc/socket/socket_address.h>
#include <string>

namespace p2psc {

class Mediator {
public:
  Mediator(const socket::SocketAddress &socket_address);

  socket::SocketAddress socket_address() const;
  std::string toString() const;

  std::shared_ptr<Socket> connect(const key::Keypair &our_keypair,
                                  const Peer &peer) const;

private:
  const socket::SocketAddress _socket_address;
};
}