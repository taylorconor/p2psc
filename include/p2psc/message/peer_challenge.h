#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct PeerChallenge {
  static const MessageType type = kTypePeerChallenge;
  std::string encrypted_nonce;
};

inline bool operator==(const PeerChallenge &lhs, const PeerChallenge &rhs) {
  return lhs.encrypted_nonce == rhs.encrypted_nonce;
}
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::PeerChallenge> {
  static codec::object_t<p2psc::message::PeerChallenge> codec() {
    auto codec = codec::object<p2psc::message::PeerChallenge>();
    codec.required("encrypted_nonce",
                   &p2psc::message::PeerChallenge::encrypted_nonce);
    return codec;
  }
};
}
}