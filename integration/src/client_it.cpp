#include <boost/test/unit_test.hpp>
#include <include/util/client.h>
#include <include/util/fake_mediator.h>
#include <p2psc/message/advertise.h>
#include <p2psc/message/advertise_challenge.h>
#include <p2psc/message/advertise_response.h>
#include <p2psc/message/message_decoder.h>

namespace p2psc {
namespace integration {

/**
 * This integration test suite tests a real Peer (wrapped in a Client helper
 * class) against a fake mediator.
 */
BOOST_AUTO_TEST_SUITE(client_integration_test)

BOOST_AUTO_TEST_CASE(ShouldSendValidAdvertise) {
  util::FakeMediator mediator;
  mediator.quit_after(message::kTypeAdvertise);
  mediator.run();

  const auto keypair = key::Keypair::generate();
  const auto peer_pub_key = key::PublicKey::generate();
  auto peer = util::Client(Peer(peer_pub_key),
                           mediator.get_mediator_description(), keypair);
  const auto socket = peer.connect();

  const auto mediator_messages = mediator.get_received_messages();
  BOOST_ASSERT(mediator_messages.size() == 1);
  const auto advertise =
      message::decode<message::Advertise>(mediator_messages[0]);
  BOOST_ASSERT(advertise.payload.our_key ==
               keypair.get_serialised_public_key());
  BOOST_ASSERT(advertise.payload.their_key == peer_pub_key.serialise());
}

BOOST_AUTO_TEST_CASE(ShouldCorrectlyProveIdentityWithNonce) {
  util::FakeMediator mediator;
  mediator.quit_after(message::kTypeAdvertiseResponse);
  mediator.run();

  const auto keypair = key::Keypair::generate();
  const auto peer_pub_key = key::PublicKey::generate();
  auto peer = util::Client(Peer(peer_pub_key),
                           mediator.get_mediator_description(), keypair);
  const auto socket = peer.connect();

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

BOOST_AUTO_TEST_SUITE_END()
}
}