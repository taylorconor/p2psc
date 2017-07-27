#include <iostream>
#include <p2psc/connection.h>
#include <p2psc/message.h>
#include <thread>
#include <p2psc/mediator_connection.h>

namespace p2psc {
void Connection::connectToPeer(const key::Keypair &our_keypair,
                               const Peer &peer, const Mediator &mediator,
                               const Callback &callback) {
  _executeSynchronously(std::bind(Connection::_handleConnection, our_keypair,
                                  peer, mediator, callback));
}

void Connection::_executeAsynchronously(std::function<void()> f) {
  std::thread thread(f);
  thread.detach();
}

void Connection::_executeSynchronously(std::function<void()> f) { f(); }

void Connection::_handleConnection(const key::Keypair &our_keypair,
                                   const Peer &peer, const Mediator &mediator,
                                   const Callback &callback) {
  try {
    std::shared_ptr<Socket> socket =
        _connectToPeer(our_keypair, peer, mediator);
  } catch (const socket::SocketException &e) {
    std::cout << "Failed to connect to peer: " << e.what() << std::endl;
    // TODO: error callback
  }
}

std::shared_ptr<Socket>
Connection::_connectToPeer(const key::Keypair &our_keypair, const Peer &peer,
                           const Mediator &mediator) {
  // Depending on who handshakes with the Mediator first, either we will have
  // to connect to the Peer (we handshake first) or the Peer will connect
  // with us (they handshake first).
  //
  // If they connect with us, we don't have to do anything; the mediator
  // connection will yield a socket with the peer over which we must then
  // verify our identities. Otherwise, we first must create a socket with the
  // Peer before verification can happen.
  auto mediator_connection = MediatorConnection(mediator);
  mediator_connection.connect(our_keypair, peer);
  if (mediator_connection.has_punched_peer()) {
    // TODO: implement this
    throw std::runtime_error("Did not expect PunchedPeer");
  } else if (mediator_connection.has_peer_challenge()) {
    // TODO: send peer challenge response
  } else {
    throw std::runtime_error("No PunchedPeer or PeerChallenge");
  }
  return mediator_connection.get_socket();
}
}