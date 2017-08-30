#include <iostream>
#include <p2psc/connection.h>
#include <p2psc/crypto/crypto_exception.h>
#include <p2psc/log.h>
#include <p2psc/mediator_connection.h>
#include <p2psc/message.h>
#include <p2psc/message/message_util.h>
#include <p2psc/message/peer_challenge_response.h>
#include <p2psc/message/peer_response.h>
#include <p2psc/message/peer_secret.h>
#include <thread>

namespace p2psc {
namespace {
std::shared_ptr<Socket>
_connect_as_client(MediatorConnection &mediator_connection,
                   const key::Keypair &our_keypair) {
  std::shared_ptr<Socket> socket =
      std::make_shared<Socket>(mediator_connection.get_punched_peer().address);

  // send peer challenge
  std::srand(std::time(0));
  const std::string nonce = std::to_string(std::rand());
  const auto encrypted_nonce =
      mediator_connection.get_punched_peer().peer.public_key.encrypt(nonce);
  const auto peer_challenge =
      Message<message::PeerChallenge>(message::PeerChallenge{encrypted_nonce});
  message::send_and_log(socket, peer_challenge);

  // receive peer challenge response
  const auto peer_challenge_response =
      message::receive_and_log<message::PeerChallengeResponse>(socket);

  if (peer_challenge_response.format().payload.decrypted_nonce !=
      encrypted_nonce) {
    throw std::runtime_error(
        "PeerChallengeResponse: peer did not pass verification");
  }
  std::string decrypted_peer_nonce;
  try {
    decrypted_peer_nonce = our_keypair.private_decrypt(
        peer_challenge_response.format().payload.encrypted_nonce);
  } catch (crypto::CryptoException &e) {
    throw std::runtime_error(
        "PeerChallengeResponse: Could not decrypt encrypted_nonce");
  }

  // send peer response
  const auto peer_response = Message<message::PeerResponse>(
      message::PeerResponse{decrypted_peer_nonce});
  message::send_and_log(socket, peer_challenge);

  // receive peer secret
  const auto peer_secret =
      message::receive_and_log<message::PeerSecret>(socket);

  // deregister this client from the mediator, using the peer's secret as
  // proof of successful peer handshake
  mediator_connection.deregister(peer_secret.format().payload.secret);

  return socket;
}
}

void Connection::connect(const key::Keypair &our_keypair, const Peer &peer,
                         const Mediator &mediator, const Callback &callback) {
  _execute_asynchronously(std::bind(Connection::_handle_connection, our_keypair,
                                    peer, mediator, callback));
}

void Connection::_execute_asynchronously(std::function<void()> f) {
  std::thread thread(f);
  thread.detach();
}

void Connection::_handle_connection(const key::Keypair &our_keypair,
                                    const Peer &peer, const Mediator &mediator,
                                    const Callback &callback) {
  try {
    std::shared_ptr<Socket> socket = _connect(our_keypair, peer, mediator);
  } catch (const socket::SocketException &e) {
    LOG(level::Error) << "Failed to connect to peer: " << e.what();
    // TODO: make an exception class specifically designed for returning an
    // error in the callback
  }
}

std::shared_ptr<Socket> Connection::_connect(const key::Keypair &our_keypair,
                                             const Peer &peer,
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
    return _connect_as_client(mediator_connection, our_keypair);
  } else if (mediator_connection.has_peer_challenge()) {
    // TODO: send peer challenge response
    LOG(level::Warning) << "Unimplemented: received PeerChallenge from mediator"
                           " connection";
    return nullptr;
  } else {
    throw std::runtime_error("No PunchedPeer or PeerChallenge");
  }
}
}