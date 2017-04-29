#include <p2psc/connection.h>
#include <thread>

namespace p2psc {
void Connection::connectToPeer(const key::Keypair &our_keypair,
                               const Peer &peer, const Mediator &mediator,
                               const Callback &callback) {
  _executeAsynchronously(std::bind(Connection::_connectToPeer, our_keypair,
                                   peer, mediator, callback));
}

void Connection::_executeAsynchronously(std::function<void()> f) {
  std::thread thread(f);
  thread.detach();
}

void Connection::_connectToPeer(const key::Keypair &our_keypair,
                                const Peer &peer, const Mediator &mediator,
                                const Callback &callback) {}
}