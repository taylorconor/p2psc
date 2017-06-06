#include <p2psc/log.h>
#include <p2psc/mediator.h>
#include <p2psc/message.h>
#include <p2psc/message/advertise.h>
#include <p2psc/message/advertise_challenge.h>
#include <p2psc/message/message_decoder.h>

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

  const auto advertise = Message<message::Advertise>(message::Advertise{
      our_keypair.get_serialised_public_key(), peer.public_key.serialise()});
  const auto raw_advertise = encode(advertise.format());
  socket->send(raw_advertise);
  LOG(level::Debug) << "Sending Advertise: " << raw_advertise;

  // receive advertise challenge
  const auto raw_advertise_challenge = socket->receive();
  const auto advertise_challenge =
      message::decode<message::AdvertiseChallenge>(raw_advertise_challenge);
  LOG(level::Debug) << "Received AdvertiseChallenge: "
                    << raw_advertise_challenge;
  const auto decrypted_nonce =
      our_keypair.private_decrypt(advertise_challenge.payload.encrypted_nonce);

  return socket;
}
}