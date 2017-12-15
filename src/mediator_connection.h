#pragma once

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <p2psc/key/keypair.h>
#include <p2psc/mediator.h>
#include <p2psc/message/peer_challenge.h>
#include <p2psc/message/peer_disconnect.h>
#include <p2psc/punched_peer.h>
#include <p2psc/socket/socket.h>
#include <p2psc/socket_creator.h>

namespace p2psc {

class MediatorConnection {
public:
  MediatorConnection(const Mediator &mediator,
                     const SocketCreator &socket_creator);

  void connect(const key::Keypair &our_keypair, const Peer &peer);
  void close_socket();

  bool has_punched_peer() const;
  PunchedPeer get_punched_peer() const;
  bool has_peer_disconnect() const;
  message::PeerDisconnect get_peer_disconnect() const;

  std::shared_ptr<Socket> get_socket() const;

private:
  const Mediator _mediator;
  bool _connected;
  boost::optional<PunchedPeer> _punched_peer;
  boost::optional<message::PeerDisconnect> _peer_disconnect;
  SocketCreator _socket_creator;
  std::shared_ptr<Socket> _socket;
};
}