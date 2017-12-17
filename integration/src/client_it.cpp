#include <boost/test/unit_test.hpp>
#include <p2psc/message/advertise.h>
#include <p2psc/message/advertise_challenge.h>
#include <p2psc/message/advertise_response.h>
#include <p2psc/message/message_decoder.h>
#include <p2psc/message/peer_identification.h>
#include <src/util/client.h>
#include <src/util/fake_mediator.h>

namespace p2psc {
namespace integration {
namespace {
const uint64_t kDefaultPeerConnectTimeout = 100;

void block(uint64_t ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

const auto stateful_socket_creator =
    [](const SocketAddressOrFileDescriptor &address_or_file_descriptor) {
      if (address_or_file_descriptor.has_socket_address()) {
        return std::make_shared<util::StatefulSocket>(
            address_or_file_descriptor.socket_address());
      } else {
        return std::make_shared<util::StatefulSocket>(
            address_or_file_descriptor.sock_fd());
      }
    };
}

/**
 * This integration test suite tests a real Peer (wrapped in a Client helper
 * class) against a fake mediator.
 */
BOOST_AUTO_TEST_SUITE(client_integration_test)

BOOST_AUTO_TEST_CASE(ShouldSendValidAdvertise) {
  util::FakeMediator mediator(stateful_socket_creator);
  mediator.quit_after(message::kTypeAdvertise);
  mediator.run();

  const auto keypair = key::Keypair::generate();
  const auto peer_pub_key = key::PublicKey::generate();
  auto peer = util::Client(Peer(peer_pub_key),
                           mediator.get_mediator_description(), keypair);
  const auto socket = peer.connect_sync(kDefaultPeerConnectTimeout);

  const auto received_messages = mediator.get_received_messages();
  BOOST_ASSERT(received_messages.size() == 1);
  BOOST_ASSERT(mediator.get_sent_messages().size() == 0);
  const auto advertise =
      message::decode<message::Advertise>(received_messages[0]);
  BOOST_ASSERT(advertise.payload.our_key ==
               keypair.get_serialised_public_key());
  BOOST_ASSERT(advertise.payload.their_key == peer_pub_key.serialise());
}

BOOST_AUTO_TEST_CASE(ShouldCorrectlyProveIdentityWithNonce) {
  util::FakeMediator mediator(stateful_socket_creator);
  mediator.quit_after(message::kTypeAdvertiseResponse);
  mediator.run();

  const auto keypair = key::Keypair::generate();
  const auto peer_pub_key = key::PublicKey::generate();
  auto peer = util::Client(Peer(peer_pub_key),
                           mediator.get_mediator_description(), keypair);
  const auto socket = peer.connect_sync(kDefaultPeerConnectTimeout);

  const auto received_messages = mediator.get_received_messages();
  BOOST_ASSERT(received_messages.size() == 2);
  const auto sent_messages = mediator.get_sent_messages();
  BOOST_ASSERT(sent_messages.size() == 1);
  const auto advertise_challenge =
      message::decode<message::AdvertiseChallenge>(sent_messages[0]);
  const auto advertise_response =
      message::decode<message::AdvertiseResponse>(received_messages[1]);
  const auto decrypted_nonce =
      keypair.private_decrypt(advertise_challenge.payload.encrypted_nonce);
  BOOST_ASSERT(decrypted_nonce == advertise_response.payload.nonce);
}

BOOST_AUTO_TEST_CASE(ShouldSendPeerIdentificationToFirstPeer) {
  util::FakeMediator mediator(stateful_socket_creator);
  mediator.quit_after(message::kTypePeerIdentification);
  mediator.run();

  const auto client_keypair = key::Keypair::generate();
  const auto peer_keypair = key::Keypair::generate();
  auto client =
      util::Client(Peer(key::PublicKey::from_string(
                       peer_keypair.get_serialised_public_key())),
                   mediator.get_mediator_description(), client_keypair);
  // the client connects first
  const auto client_socket = client.connect_async();
  block(kDefaultPeerConnectTimeout);
  auto peer = util::Client(Peer(key::PublicKey::from_string(
                               client_keypair.get_serialised_public_key())),
                           mediator.get_mediator_description(), peer_keypair);
  const auto peer_socket = peer.connect_async();
  mediator.await_shutdown();
  block(kDefaultPeerConnectTimeout);

  BOOST_ASSERT(client_socket != nullptr);
  BOOST_ASSERT(peer_socket != nullptr);

  const auto sent_messages = mediator.get_sent_messages();
  BOOST_ASSERT(sent_messages.size() == 4);
  BOOST_ASSERT(message::decode_message_type(sent_messages[2]) ==
               message::kTypePeerDisconnect);
  BOOST_ASSERT(message::decode_message_type(sent_messages[3]) ==
               message::kTypePeerIdentification);

  const auto peer_identification =
      message::decode<message::PeerIdentification>(sent_messages[3]);
  BOOST_ASSERT(peer_identification.payload.port ==
               client_socket->get_socket_address().port());
  BOOST_ASSERT(peer_identification.payload.ip ==
               client_socket->get_socket_address().ip());
}

BOOST_AUTO_TEST_CASE(ShouldCompletePeerHandshake) {
  util::FakeMediator mediator(stateful_socket_creator);
  mediator.run();

  const auto client_keypair = key::Keypair::generate();
  const auto peer_keypair = key::Keypair::generate();
  auto client =
      util::Client(Peer(key::PublicKey::from_string(
                       peer_keypair.get_serialised_public_key())),
                   mediator.get_mediator_description(), client_keypair);
  // the client connects first
  const auto client_socket = client.connect_async();
  block(kDefaultPeerConnectTimeout);
  auto peer = util::Client(Peer(key::PublicKey::from_string(
                               client_keypair.get_serialised_public_key())),
                           mediator.get_mediator_description(), peer_keypair);
  const auto peer_socket = peer.connect_async();
  mediator.await_shutdown();
  block(kDefaultPeerConnectTimeout);

  BOOST_ASSERT(client_socket->get_sent_messages().size() == 2);
  BOOST_ASSERT(peer_socket->get_sent_messages().size() == 2);
  BOOST_ASSERT(client_socket->get_received_messages().size() == 2);
  BOOST_ASSERT(peer_socket->get_received_messages().size() == 2);

  const auto message_type =
      message::decode_message_type(client_socket->get_sent_messages()[0]);
  BOOST_ASSERT(message_type == message::kTypePeerChallenge);

  const auto message = "bananarama!";
  client_socket->send(message);
  const auto received_message = peer_socket->receive();
  BOOST_ASSERT(received_message == message);
}

BOOST_AUTO_TEST_SUITE_END()
}
}