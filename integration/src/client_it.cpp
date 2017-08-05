#include <boost/test/unit_test.hpp>
#include <include/util/client.h>
#include <include/util/fake_mediator.h>
#include <p2psc/message/advertise.h>
#include <p2psc/message/message_decoder.h>

namespace p2psc {
namespace integration {

BOOST_AUTO_TEST_SUITE(client_integration_test)

BOOST_AUTO_TEST_CASE(ShouldSendValidAdvertise) {
  util::FakeMediator mediator;
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

BOOST_AUTO_TEST_CASE(ShouldDoAThing) {
  try {
    message::decode_message_type("banana");
  } catch (std::exception e) {
    // TODO: fix decode_message_type unused function error!
  }
}

BOOST_AUTO_TEST_SUITE_END()
}
}