#pragma once

#include <p2psc/key/keypair.h>
#include <p2psc/mediator.h>
#include <p2psc/peer.h>
#include <p2psc/socket.h>

namespace p2psc {
namespace integration {
namespace util {

class Client {
public:
  Client(const p2psc::Peer &peer, const p2psc::Mediator &mediator,
         const p2psc::key::Keypair &keypair)
      : _peer(peer), _mediator(mediator), _keypair(keypair) {}

  std::shared_ptr<Socket> connect();

private:
  p2psc::Peer _peer;
  p2psc::Mediator _mediator;
  p2psc::key::Keypair _keypair;
};
}
}
}