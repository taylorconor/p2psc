#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct PeerChallengeResponse {
  static const MessageType type = kTypePeerChallengeResponse;
  std::string encrypted_nonce;
  std::string decrypted_nonce;
};
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::PeerChallengeResponse> {
  static codec::object_t<p2psc::message::PeerChallengeResponse> codec() {
    auto codec = codec::object<p2psc::message::PeerChallengeResponse>();
    codec.required("encrypted_nonce",
                   &p2psc::message::PeerChallengeResponse::encrypted_nonce);
    codec.required("decrypted_nonce",
                   &p2psc::message::PeerChallengeResponse::decrypted_nonce);
    return codec;
  }
};
}
}