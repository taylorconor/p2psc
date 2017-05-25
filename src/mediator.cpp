#include <p2psc/mediator.h>
#include <p2psc/message.h>
#include <p2psc/message/advertise.h>

namespace p2psc {
Mediator::Mediator(const socket::SocketAddress &socket_address)
    : _socket_address(socket_address) {}

socket::SocketAddress Mediator::socket_address() const {
  return _socket_address;
}

std::string Mediator::toString() const {
  return _socket_address.ip + ":" + std::to_string(_socket_address.port);
}

boost::variant<std::shared_ptr<Socket>, PunchedPeer>
Mediator::connect(const key::Keypair &our_keypair, const Peer &peer) const {
  std::shared_ptr<Socket> socket = std::make_shared<Socket>(_socket_address);

  // TODO: use data from our_keypair and peer
  const auto advertise = Message<message::Advertise>(
      message::Advertise{"our_test_key", "their_test_key"});

  socket->send(encode(advertise.format()));

  // receive advertise response
  const auto raw_advertise_response = socket->receive();
  std::cout << "Received advertise response: " << raw_advertise_response
            << std::endl;

  return socket;
}
}