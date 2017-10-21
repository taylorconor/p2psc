#pragma once

#include <cstdint>

namespace p2psc {
namespace message {

using MessageType = std::uint8_t;

static const MessageType kTypeAdvertise = 0;
static const MessageType kTypeAdvertiseChallenge = 1;
static const MessageType kTypeAdvertiseResponse = 2;
static const MessageType kTypePeerDisconnect = 3;
static const MessageType kTypePeerIdentification = 4;
static const MessageType kTypePeerChallenge = 5;
static const MessageType kTypePeerChallengeResponse = 6;
static const MessageType kTypePeerResponse = 7;

inline std::string message_type_string(MessageType type) {
  switch (type) {
  case kTypeAdvertise:
    return "Advertise";
  case kTypeAdvertiseChallenge:
    return "AdvertiseChallenge";
  case kTypeAdvertiseResponse:
    return "AdvertiseResponse";
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
  default:
    return "Unknown (" + std::to_string(type) + ")";
  }
}
}
}