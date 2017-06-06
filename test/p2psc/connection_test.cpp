#include <boost/test/unit_test.hpp>
#include <iostream>
#include <p2psc/connection.h>

namespace p2psc {
namespace test {

BOOST_AUTO_TEST_SUITE(connection_test)

BOOST_AUTO_TEST_CASE(ShouldDoSomething) {
  const auto keypair = key::Keypair::generate();
  const auto peer_pub_key = key::PublicKey::generate();
  const auto peer = Peer(peer_pub_key);
  const auto mediator = Mediator(socket::SocketAddress("127.0.0.1", 1337));
  const auto callback = []() { std::cout << "Banana" << std::endl; };
  Connection::connectToPeer(keypair, peer, mediator, callback);
}

BOOST_AUTO_TEST_SUITE_END()
}
}