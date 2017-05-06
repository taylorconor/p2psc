#include <boost/test/unit_test.hpp>
#include <iostream>
#include <p2psc/message.h>
#include <p2psc/message/advertise.h>

namespace p2psc {
namespace test {

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_CASE(ShouldCreateMessageWithCorrectVersion) {
  const auto message = Message<message::Advertise>(message::Advertise {
      "our_test_key",
      "their_test_key"
  });
  BOOST_ASSERT(message.format().version == kVersion);
}

BOOST_AUTO_TEST_CASE(ShouldCreateMessageWithCorrectType) {
  const auto message = Message<message::Advertise>(message::Advertise {
      "our_test_key",
      "their_test_key"
  });
  BOOST_ASSERT(message.format().type == message::kTypeAdvertise);
}

BOOST_AUTO_TEST_SUITE_END()
}
}