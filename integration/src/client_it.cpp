#include <boost/test/unit_test.hpp>
#include <include/util/client.h>
#include <include/util/fake_mediator.h>

namespace p2psc {
namespace integration {

BOOST_AUTO_TEST_SUITE(client_integration_test)

BOOST_AUTO_TEST_CASE(ShouldDoAThing) {
  util::FakeMediator mediator;
  mediator.run();

  const auto keypair = key::Keypair::generate();
  const auto peer_pub_key = key::PublicKey::generate();
  auto peer = util::Client(Peer(peer_pub_key),
                           mediator.get_mediator_description(), keypair);
  const auto socket = peer.connect();

  std::cout << "Running integration test suite" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
}
}