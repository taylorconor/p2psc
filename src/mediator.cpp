#include <p2psc/crypto/crypto_exception.h>
#include <p2psc/log.h>
#include <p2psc/mediator.h>
#include <p2psc/message.h>
#include <p2psc/message/advertise.h>
#include <p2psc/message/advertise_challenge.h>
#include <p2psc/message/advertise_response.h>
#include <p2psc/message/message_decoder.h>

namespace p2psc {
namespace {
template <class T>
void send_and_log(std::shared_ptr<Socket> socket, const Message<T> &message) {
  const auto json = encode(message.format());
  socket->send(json);
  const auto message_type = message::get_message_type(message.format().type);
  LOG(level::Debug) << "Sending " << message_type << ": " << json;
}

template <class T> Message<T> receive_and_log(std::shared_ptr<Socket> socket) {
  const auto raw_message = socket->receive();
  auto message = message::decode<T>(raw_message);
  const auto message_type = message::get_message_type(message.type);
  LOG(level::Debug) << "Received " << message_type << ": " << raw_message;
  return message.payload;
}
}
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

  // send advertise
  const auto advertise = Message<message::Advertise>(message::Advertise{
      our_keypair.get_serialised_public_key(), peer.public_key.serialise()});
  send_and_log(socket, advertise);

  // receive advertise challenge
  const auto advertise_challenge =
      receive_and_log<message::AdvertiseChallenge>(socket);

  std::string decrypted_nonce;
  try {
    decrypted_nonce = our_keypair.private_decrypt(
        advertise_challenge.format().payload.encrypted_nonce);
  } catch (crypto::CryptoException &e) {
    throw std::runtime_error(
        "AdvertiseChallenge: Could not decrypt encrypted_nonce");
  }

  // send advertise response
  const auto advertise_response = Message<message::AdvertiseResponse>(
      message::AdvertiseResponse{decrypted_nonce});
  send_and_log(socket, advertise_response);

  return socket;
}
}