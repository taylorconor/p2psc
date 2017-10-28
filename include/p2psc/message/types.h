#pragma once

#include <cstdint>

namespace p2psc {
namespace message {

using MessageType = std::uint8_t;

static const MessageType kTypeAdvertise = 0;
static const MessageType kTypeAdvertiseChallenge = 1;
static const MessageType kTypeAdvertiseResponse = 2;
static const MessageType kTypeAdvertiseAbort = 3;
static const MessageType kTypeAdvertiseRetry = 4;
static const MessageType kTypePeerDisconnect = 5;
static const MessageType kTypePeerIdentification = 6;
static const MessageType kTypePeerChallenge = 7;
static const MessageType kTypePeerChallengeResponse = 8;
static const MessageType kTypePeerResponse = 9;
static const MessageType kTypePeerAcknowledgement = 10;

inline std::string message_type_string(MessageType type) {
  switch (type) {
  case kTypeAdvertise:
    return "Advertise";
  case kTypeAdvertiseChallenge:
    return "AdvertiseChallenge";
  case kTypeAdvertiseResponse:
    return "AdvertiseResponse";
  case kTypeAdvertiseAbort:
    return "AdvertiseAbort";
  case kTypeAdvertiseRetry:
    return "AdvertiseRetry";
  case kTypePeerDisconnect:
    return "PeerDisconnect";
  case kTypePeerIdentification:
    return "PeerIdentification";
  case kTypePeerChallenge:
    return "PeerChallenge";
  case kTypePeerChallengeResponse:
    return "PeerChallengeResponse";
  case kTypePeerResponse:
    return "PeerResponse";
  case kTypePeerAcknowledgement:
    return "PeerAcknowledgement";
  default:
    return "Unknown (" + std::to_string(type) + ")";
  }
}
}
}