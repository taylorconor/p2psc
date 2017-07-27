#pragma once

#include <p2psc/mediator.h>
#include <p2psc/key/keypair.h>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <p2psc/message/peer_challenge.h>
#include <p2psc/socket.h>
#include <p2psc/punched_peer.h>

namespace p2psc {

class MediatorConnection {
public:
  MediatorConnection(const Mediator &mediator);

  boost::variant<std::shared_ptr<Socket>, PunchedPeer>
  connect(const key::Keypair &our_keypair, const Peer &peer);

private:
  const Mediator _mediator;
  bool _connected;
  boost::optional<PunchedPeer> _punched_peer;
  boost::optional<message::PeerChallenge> _peer_challenge;
  std::shared_ptr<Socket> _socket;
  std::string _challenge_secret;
};
}