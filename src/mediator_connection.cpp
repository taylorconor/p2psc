#include "mediator_connection.h"

#include <p2psc/crypto/crypto_exception.h>
#include <p2psc/log.h>
#include <p2psc/message/advertise.h>
#include <p2psc/message/advertise_abort.h>
#include <p2psc/message/advertise_challenge.h>
#include <p2psc/message/advertise_response.h>
#include <p2psc/message/advertise_retry.h>
#include <p2psc/message/message.h>
#include <p2psc/message/message_decoder.h>
#include <p2psc/message/message_util.h>
#include <p2psc/message/peer_identification.h>

namespace p2psc {
namespace {
const int MAX_ADVERTISE_RETRIES = 5;
}

MediatorConnection::MediatorConnection(const Mediator &mediator,
                                       const SocketCreator &socket_creator)
    : _mediator(mediator), _connected(false), _socket_creator(socket_creator),
      _socket(nullptr) {}

void MediatorConnection::connect(const key::Keypair &our_keypair,
                                 const Peer &peer) {
  BOOST_ASSERT(!_connected);
  LOG(level::Info) << "Connecting to Mediator (on " << _mediator.socket_address
                   << ")";
  _socket = _socket_creator(_mediator.socket_address);

  message::MessageType mediator_response_type;
  message::AdvertiseChallenge advertise_challenge;
  int advertise_retries = 0;
  do {
    // send advertise
    const auto advertise = Message<message::Advertise>(
        message::Advertise{kVersion, our_keypair.get_serialised_public_key(),
                           peer.public_key.serialise()});
    message::send_and_log(_socket, advertise);

    // receive some response from the mediator
    const auto raw_mediator_response = _socket->receive();
    mediator_response_type =
        message::decode_message_type(raw_mediator_response);
    if (mediator_response_type == message::kTypeAdvertiseAbort) {
      const auto advertise_abort =
          message::decode<message::AdvertiseAbort>(raw_mediator_response);
      message::log_message(advertise_abort, _socket->get_socket_address());
      throw std::runtime_error("AdvertiseAbort: " +
                               advertise_abort.payload.reason);
    } else if (mediator_response_type == message::kTypeAdvertiseRetry) {
      const auto advertise_retry =
          message::decode<message::AdvertiseRetry>(raw_mediator_response);
      message::log_message(advertise_retry, _socket->get_socket_address());
      if (advertise_retries == MAX_ADVERTISE_RETRIES) {
        throw std::runtime_error(
            "AdvertiseRetry: Retried " + std::to_string(advertise_retries) +
            " times, aborting. Reason: " + advertise_retry.payload.reason);
      }
      LOG(level::Info) << "Advertise rejected by Mediator. Retrying. Reason: "
                       << advertise_retry.payload.reason;
      advertise_retries++;
    } else if (mediator_response_type == message::kTypeAdvertiseChallenge) {
      const auto advertise_challenge_message =
          message::decode<message::AdvertiseChallenge>(raw_mediator_response);
      message::log_message(advertise_challenge_message,
                           _socket->get_socket_address());
      advertise_challenge =
          message::decode<message::AdvertiseChallenge>(raw_mediator_response)
              .payload;
    } else {
      throw std::runtime_error(
          "Unexpected message type: " +
          message::message_type_string(mediator_response_type));
    }
  } while (mediator_response_type == message::kTypeAdvertiseRetry);

  std::string decrypted_nonce;
  try {
    decrypted_nonce =
        our_keypair.private_decrypt(advertise_challenge.encrypted_nonce);
  } catch (crypto::CryptoException &e) {
    throw std::runtime_error(
        "AdvertiseChallenge: Could not decrypt encrypted_nonce");
  }

  // send advertise response
  const auto advertise_response = Message<message::AdvertiseResponse>(
      message::AdvertiseResponse{decrypted_nonce});
  message::send_and_log(_socket, advertise_response);

  // now we wait for either a PeerIdentification or PeerChallenge.
  const auto raw_message = _socket->receive();
  const auto message_type = message::decode_message_type(raw_message);
  if (message_type == message::kTypePeerIdentification) {
    const auto peer_identification =
        message::decode<message::PeerIdentification>(raw_message);
    message::log_message(peer_identification, _socket->get_socket_address());
    const auto socket_address = socket::SocketAddress(
        peer_identification.payload.ip, peer_identification.payload.port);
    _punched_peer =
        PunchedPeer(peer, socket_address, peer_identification.payload.version);
    _connected = true;
  } else if (message_type == message::kTypePeerDisconnect) {
    const auto peer_disconnect =
        message::decode<message::PeerDisconnect>(raw_message);
    message::log_message(peer_disconnect, _socket->get_socket_address());
    _peer_disconnect = peer_disconnect.payload;
    _connected = true;
  } else {
    LOG(level::Error) << "Received unexpected message type: "
                      << message::message_type_string(message_type) << ": "
                      << raw_message;
  }
}

void MediatorConnection::close_socket() {
  BOOST_ASSERT(_connected);
  _socket->close();
}

bool MediatorConnection::has_punched_peer() const {
  return _punched_peer.is_initialized();
}

PunchedPeer MediatorConnection::get_punched_peer() const {
  BOOST_ASSERT(_punched_peer);
  return *_punched_peer;
}

bool MediatorConnection::has_peer_disconnect() const {
  return _peer_disconnect.is_initialized();
}

message::PeerDisconnect MediatorConnection::get_peer_disconnect() const {
  BOOST_ASSERT(_peer_disconnect);
  return *_peer_disconnect;
}

std::shared_ptr<Socket> MediatorConnection::get_socket() const {
  return _socket;
}
}