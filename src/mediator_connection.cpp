#include <p2psc/crypto/crypto_exception.h>
#include <p2psc/log.h>
#include <p2psc/mediator_connection.h>
#include <p2psc/message.h>
#include <p2psc/message/advertise.h>
#include <p2psc/message/advertise_challenge.h>
#include <p2psc/message/advertise_response.h>
#include <p2psc/message/message_decoder.h>
#include <p2psc/message/peer_identification.h>

namespace p2psc {
namespace {
template <class T>
void send_and_log(std::shared_ptr<Socket> socket, const Message<T> &message) {
  const auto json = encode(message.format());
  socket->send(json);
  const auto message_type = message::message_type_string(message.format().type);
  LOG(level::Debug) << "Sending " << message_type << ": " << json;
}

template <class T> Message<T> receive_and_log(std::shared_ptr<Socket> socket) {
  const auto raw_message = socket->receive();
  auto message = message::decode<T>(raw_message);
  const auto message_type = message::message_type_string(message.type);
  LOG(level::Debug) << "Received " << message_type << ": " << raw_message;
  return message.payload;
}
}
MediatorConnection::MediatorConnection(const Mediator &mediator)
    : _mediator(mediator), _connected(false), _socket(nullptr) {}

boost::variant<std::shared_ptr<Socket>, PunchedPeer>
MediatorConnection::connect(const key::Keypair &our_keypair, const Peer &peer) {
  BOOST_ASSERT(!_connected);
  _socket = std::make_shared<Socket>(_mediator.socket_address);

  // send advertise
  const auto advertise = Message<message::Advertise>(message::Advertise{
      our_keypair.get_serialised_public_key(), peer.public_key.serialise()});
  send_and_log(_socket, advertise);

  // receive advertise challenge
  const auto advertise_challenge =
      receive_and_log<message::AdvertiseChallenge>(_socket);
  _challenge_secret = advertise_challenge.format().payload.secret;

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
  send_and_log(_socket, advertise_response);

  // now we wait for either a PeerIdentification message from the Mediator,
  // or for a PeerChallenge from the Peer.
  const auto raw_message = _socket->receive();
  const auto message_type = message::decode_message_type(raw_message);
  if (message_type == message::kTypePeerIdentification) {
    LOG(level::Debug) << "Received PeerIdentification: " << raw_message;
    const auto peer_identification =
        message::decode<message::PeerIdentification>(raw_message);
    const auto socket_address = socket::SocketAddress(
        peer_identification.payload.ip, peer_identification.payload.port);
    _punched_peer = PunchedPeer(peer, socket_address);
  } else if (message_type == message::kTypePeerChallenge) {
    LOG(level::Debug) << "Received PeerChallenge: " << raw_message;
  } else {
    LOG(level::Error) << "Received unexpected message type: "
                      << message::message_type_string(message_type) << ": "
                      << raw_message;
  }

  return _socket;
}
}