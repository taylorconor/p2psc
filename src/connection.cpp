#include <iostream>
#include <p2psc/connection.h>
#include <p2psc/message.h>
#include <p2psc/message/advertise.h>
#include <thread>

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
  std::shared_ptr<Socket> socket = mediator.connect(our_keypair, peer);
  return socket;
}
}