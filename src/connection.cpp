#include <iostream>
#include <p2psc/connection.h>
#include <p2psc/crypto/crypto_exception.h>
#include <p2psc/log.h>
#include <p2psc/mediator_connection.h>
#include <p2psc/message.h>
#include <p2psc/message/message_util.h>
#include <p2psc/message/peer_challenge_response.h>
#include <p2psc/message/peer_response.h>
#include <p2psc/socket/local_listening_socket.h>

namespace p2psc {
namespace {
std::string generate_nonce() {
  std::srand(std::time(0));
  return std::to_string(std::rand());
}

std::shared_ptr<Socket>
_connect_as_client(MediatorConnection &mediator_connection,
                   const key::Keypair &our_keypair) {
  LOG(level::Debug) << "Attempting connection as Client";
  std::shared_ptr<Socket> socket =
      std::make_shared<Socket>(mediator_connection.get_punched_peer().address);

  // send peer challenge
  const std::string nonce = generate_nonce();
  const auto encrypted_nonce =
      mediator_connection.get_punched_peer().peer.public_key.encrypt(nonce);
  const auto peer_challenge =
      Message<message::PeerChallenge>(message::PeerChallenge{encrypted_nonce});
  message::send_and_log(socket, peer_challenge);

  // receive peer challenge response
  const auto peer_challenge_response =
      message::receive_and_log<message::PeerChallengeResponse>(socket);

  if (peer_challenge_response.format().payload.decrypted_nonce != nonce) {
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
  message::send_and_log(socket, peer_response);

  return socket;
}

std::shared_ptr<Socket>
_connect_as_peer(MediatorConnection &mediator_connection,
                 const key::Keypair &our_keypair, const Peer &peer) {
  LOG(level::Debug) << "Attempting connection as Peer";
  // close mediator socket and create new socket to listen for peer challenge
  const auto socket_address = socket::SocketAddress(
      socket::local_ip, mediator_connection.get_peer_disconnect().port);
  LOG(level::Debug) << "Closing mediator socket to begin listening on "
                    << socket_address;
  mediator_connection.close();
  const auto listening_socket = socket::LocalListeningSocket(
      mediator_connection.get_peer_disconnect().port);
  const auto socket = listening_socket.accept();

  // receive peer challenge
  const auto peer_challenge =
      message::receive_and_log<message::PeerChallenge>(socket);
  const auto decrypted_nonce = our_keypair.private_decrypt(
      peer_challenge.format().payload.encrypted_nonce);

  // send peer challenge response
  const auto client_nonce = generate_nonce();
  const auto encrypted_client_nonce = peer.public_key.encrypt(client_nonce);
  const auto peer_challenge_response = Message<message::PeerChallengeResponse>(
      message::PeerChallengeResponse{encrypted_client_nonce, decrypted_nonce});
  message::send_and_log(socket, peer_challenge_response);

  // receive peer response
  const auto peer_response =
      message::receive_and_log<message::PeerResponse>(socket);
  if (peer_response.format().payload.decrypted_nonce != client_nonce) {
    throw std::runtime_error("PeerResponse: peer did not pass verification");
  }

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
  } else if (mediator_connection.has_peer_disconnect()) {
    return _connect_as_peer(mediator_connection, our_keypair, peer);
  } else {
    throw std::runtime_error("No PunchedPeer or PeerChallenge");
  }
}
}