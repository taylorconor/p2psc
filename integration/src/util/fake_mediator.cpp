#include <include/util/fake_mediator.h>
#include <p2psc/crypto/rsa.h>
#include <p2psc/log.h>
#include <p2psc/message/advertise.h>
#include <p2psc/message/advertise_challenge.h>
#include <p2psc/message/advertise_response.h>
#include <p2psc/message/message_decoder.h>
#include <p2psc/message/peer_identification.h>

#define QUIT_IF_REQUESTED(message_type, quit_indicator)                        \
  if (message_type == quit_indicator) {                                        \
    LOG(level::Debug) << "Finishing connection handling (after "               \
                      << message::message_type_string(quit_indicator) << ")";  \
    return;                                                                    \
  }

namespace p2psc {
namespace integration {
namespace util {

FakeMediator::FakeMediator() : _mediator(_socket.get_socket_address()) {}

FakeMediator::FakeMediator(const p2psc::Mediator &mediator)
    : _mediator(mediator) {}

FakeMediator::~FakeMediator() {
  if (_is_running) {
    stop();
  }
}

void FakeMediator::run() {
  BOOST_ASSERT(!_is_running);
  _is_running = true;
  _worker_thread = std::thread(&FakeMediator::_run, this);
}

void FakeMediator::stop() {
  BOOST_ASSERT(_is_running);
  _is_running = false;
  _socket.close();
  _worker_thread.join();
  for (auto &handler_thread : _handler_pool) {
    handler_thread.join();
  }
}

void FakeMediator::quit_after(message::MessageType message_type) {
  _quit_after = message_type;
}

void FakeMediator::_run() {
  while (_is_running) {
    auto socket = _socket.accept();
    if (!socket) {
      continue;
    }
    _handler_pool.emplace_back(
        std::thread(&FakeMediator::_handle_connection, this, socket));
  }
}

void FakeMediator::_handle_connection(std::shared_ptr<Socket> session_socket) {
  /*
   * Advertise
   */
  const auto advertise = _receive_and_log<message::Advertise>(session_socket);
  QUIT_IF_REQUESTED(advertise.format().type, _quit_after);

  /*
   * AdvertiseChallenge
   */
  const auto nonce = 1337;
  const auto peer_pub_key =
      crypto::RSA::from_public_key(advertise.format().payload.our_key);
  const auto advertise_challenge =
      Message<message::AdvertiseChallenge>(message::AdvertiseChallenge{
          peer_pub_key->public_encrypt(std::to_string(nonce)),
          "secret_banana"});
  _send_and_log(session_socket, advertise_challenge);
  QUIT_IF_REQUESTED(advertise_challenge.format().type, _quit_after);

  /*
   * AdvertiseResponse
   */
  const auto advertise_response =
      _receive_and_log<message::AdvertiseResponse>(session_socket);
  QUIT_IF_REQUESTED(advertise_response.format().type, _quit_after);

  const auto maybe_peer =
      _id_to_address_store.get(advertise.format().payload.their_key);
  if (!maybe_peer) {
    // In this case, this peer is the Client, and we are waiting for the Peer to
    // come online. We store the Clients address and wait for the other peer to
    // come online so we can send a PeerIdentification back to the Client.
    _id_to_address_store.put(advertise.format().payload.our_key,
                             session_socket->get_socket_address());
    // Timeout after 2 seconds
    const auto awaited_peer =
        _id_to_address_store.await(advertise.format().payload.their_key, 2000);
    if (!awaited_peer) {
      // if we never receive an awaited peer, we can't continue
      LOG(level::Error) << "Never received Advertise from peer: "
                        << advertise.format().payload.their_key;
      return;
    }

    /*
     * PeerIdentification
     */
    const auto socket_address = session_socket->get_socket_address();
    const auto peer_identification = Message<message::PeerIdentification>(
        message::PeerIdentification{awaited_peer->ip(), awaited_peer->port()});
    _send_and_log(session_socket, peer_identification);
    QUIT_IF_REQUESTED(peer_identification.format().type, _quit_after);
  } else {
    // In this case, this peer is the Peer, since the Client has already come
    // online. The Mediator is done with this peer now.
    _id_to_address_store.put(advertise.format().payload.our_key,
                             session_socket->get_socket_address());
    LOG(level::Debug) << "Registered Peer. Client is already registered";
    sleep(1);
  }
}

template <class T>
void FakeMediator::_send_and_log(std::shared_ptr<Socket> socket,
                                 const Message<T> &message) {
  const auto json = encode(message.format());
  socket->send(json);
  const auto message_type = message::message_type_string(message.format().type);
  _sent_messages.push_back(json);
  LOG(level::Debug) << "Sending " << message_type << " to "
                    << socket->get_socket_address().ip() << ":"
                    << socket->get_socket_address().port() << ": "
                    << json;
}

template <class T>
Message<T> FakeMediator::_receive_and_log(std::shared_ptr<Socket> socket) {
  const auto raw_message = socket->receive();
  auto message = message::decode<T>(raw_message);
  const auto message_type = message::message_type_string(message.type);
  _received_messages.push_back(raw_message);
  LOG(level::Debug) << "Received " << message_type << " from "
                    << socket->get_socket_address().ip() << ":"
                    << socket->get_socket_address().port() << ": "
                    << raw_message;
  return message.payload;
}

p2psc::Mediator FakeMediator::get_mediator_description() const {
  return _mediator;
}

std::vector<std::string> FakeMediator::get_received_messages() const {
  return _received_messages;
}

std::vector<std::string> FakeMediator::get_sent_messages() const {
  return _sent_messages;
}
}
}
}