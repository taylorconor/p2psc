#include <boost/test/unit_test.hpp>
#include <iostream>
#include <p2psc.h>

namespace p2psc {
namespace test {

BOOST_AUTO_TEST_SUITE(connection_test)

BOOST_AUTO_TEST_CASE(ShouldCallbackWithErrorForNonExistantMediator) {
  std::mutex mutex;
  std::condition_variable cv;
  bool has_called_callback = false;

  const auto keypair = key::Keypair::generate();
  const auto peer_pub_key = key::PublicKey::generate();
  const auto peer = Peer(peer_pub_key);
  const auto mediator = Mediator("127.0.0.1", 1337);
  const auto callback = [&](Error error, std::shared_ptr<Socket> socket) {
    BOOST_ASSERT(error);
    BOOST_ASSERT(error.kind() == error::kErrorMediatorConnectFailure);
    BOOST_ASSERT(socket == nullptr);
    has_called_callback = true;
    cv.notify_one();
  };
  p2psc::connect(keypair, peer, mediator, callback);
  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock);
  BOOST_ASSERT(has_called_callback);
}

BOOST_AUTO_TEST_SUITE_END()
}
}