#include <boost/test/unit_test.hpp>
#include <iostream>
#include <p2psc/message/advertise.h>
#include <p2psc/message/advertise_abort.h>
#include <p2psc/message/advertise_challenge.h>
#include <p2psc/message/advertise_response.h>
#include <p2psc/message/advertise_retry.h>
#include <p2psc/message/message.h>
#include <p2psc/message/message_decoder.h>
#include <p2psc/message/peer_acknowledgement.h>
#include <p2psc/message/peer_challenge.h>
#include <p2psc/message/peer_challenge_response.h>
#include <p2psc/message/peer_disconnect.h>
#include <p2psc/message/peer_identification.h>
#include <p2psc/message/peer_response.h>

namespace p2psc {
namespace test {
namespace {
template <typename T> void verifySerialisation(T test_message) {
  const auto message = Message<T>(test_message);
  const std::string serialised_message = encode(message.format());
  const auto deserialised_message = message::decode<T>(serialised_message);

  BOOST_ASSERT(deserialised_message.type == message.format().type);
  BOOST_ASSERT(deserialised_message.payload == message.format().payload);
}

template <typename T> void verifySerialisationWithoutPayload(T test_message) {
  const auto message = Message<T>(test_message);
  const std::string serialised_message = encode(message.format());
  const auto deserialised_message = message::decode<T>(serialised_message);

  BOOST_ASSERT(deserialised_message.type == message.format().type);
}
}

BOOST_AUTO_TEST_SUITE(message_test)

BOOST_AUTO_TEST_CASE(ShouldCreateMessageWithCorrectProperties) {
  const auto message = Message<message::Advertise>(
      message::Advertise{kVersion, "our_test_key", "their_test_key"});

  BOOST_ASSERT(message.format().payload.version == kVersion);
  BOOST_ASSERT(message.format().payload.our_key == "our_test_key");
  BOOST_ASSERT(message.format().payload.their_key == "their_test_key");
}

BOOST_AUTO_TEST_CASE(ShouldCreateMessageWithCorrectType) {
  const auto message = Message<message::Advertise>(
      message::Advertise{kVersion, "our_test_key", "their_test_key"});

  BOOST_ASSERT(message.format().type == message::kTypeAdvertise);
}

BOOST_AUTO_TEST_CASE(ShouldSerialiseAndDeserialiseAllMessageTypes) {
  verifySerialisation(
      message::Advertise{kVersion, "our_test_key", "their_test_key"});
  verifySerialisation(message::AdvertiseChallenge{"test_encrypted_nonce"});
  verifySerialisation(message::AdvertiseResponse{"test_nonce"});
  verifySerialisation(message::AdvertiseAbort{"test_reason"});
  verifySerialisation(message::AdvertiseRetry{"test_reason"});
  verifySerialisation(message::PeerDisconnect{1});
  verifySerialisation(message::PeerIdentification{1, "127.0.0.1", 1337});
  verifySerialisation(message::PeerChallenge{"test_encrypted_nonce"});
  verifySerialisation(message::PeerChallengeResponse{"test_encrypted_nonce",
                                                     "test_decrypted_nonce"});
  verifySerialisation(message::PeerResponse{"test_decrypted_nonce"});
  verifySerialisationWithoutPayload(message::PeerAcknowledgement{});
}

BOOST_AUTO_TEST_SUITE_END()
}
}