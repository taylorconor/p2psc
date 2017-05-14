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
  std::shared_ptr<Socket> socket;
  try {
    _connectToPeer(socket, our_keypair, peer, mediator);
  } catch (const socket::SocketException &e) {
    std::cout << "Failed to connect to peer: " << e.what() << std::endl;
    // TODO: error callback
  }
}

void Connection::_connectToPeer(std::shared_ptr<Socket> socket,
                                const key::Keypair &our_keypair,
                                const Peer &peer, const Mediator &mediator) {
  socket = std::make_shared<Socket>(Socket(mediator.ip, mediator.port));

  const auto advertise = Message<message::Advertise>(
      message::Advertise{"our_test_key", "their_test_key"});

  socket->send(encode(advertise.format()));

  // receive advertise response
  const auto raw_advertise_response = socket->receive();
  std::cout << "Received advertise response: " << raw_advertise_response
            << std::endl;
}
}