#pragma once

#include <cstdint>

namespace p2psc {
namespace message {

using MessageType = std::uint8_t;

static const MessageType kTypeAdvertise = 0;
static const MessageType kTypeAdvertiseChallenge = 1;
static const MessageType kTypeAdvertiseResponse = 2;
static const MessageType kTypePeerIdentification = 3;
static const MessageType kTypePeerChallenge = 4;
static const MessageType kTypePeerChallengeResponse = 5;
static const MessageType kTypePeerResponse = 6;
static const MessageType kTypePeerSecret = 7;
static const MessageType kTypeDeregister = 8;

inline std::string message_type_string(MessageType type) {
  switch (type) {
  case kTypeAdvertise:
    return "Advertise";
  case kTypeAdvertiseChallenge:
    return "AdvertiseChallenge";
  case kTypeAdvertiseResponse:
    return "AdvertiseResponse";
  case kTypePeerIdentification:
    return "PeerIdentification";
  case kTypePeerChallenge:
    return "PeerChallenge";
  case kTypePeerChallengeResponse:
    return "PeerChallengeResponse";
  case kTypePeerResponse:
    return "PeerResponse";
  case kTypePeerSecret:
    return "PeerSecret";
  case kTypeDeregister:
    return "Deregister";
  default:
    return "Unknown (" + std::to_string(type) + ")";
  }
}
}
}