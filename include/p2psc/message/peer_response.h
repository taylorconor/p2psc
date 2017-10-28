#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct PeerResponse {
  static const MessageType type = kTypePeerResponse;
  std::string decrypted_nonce;
};

inline bool operator==(const PeerResponse &lhs, const PeerResponse &rhs) {
  return lhs.decrypted_nonce == rhs.decrypted_nonce;
}
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::PeerResponse> {
  static codec::object_t<p2psc::message::PeerResponse> codec() {
    auto codec = codec::object<p2psc::message::PeerResponse>();
    codec.required("decrypted_nonce",
                   &p2psc::message::PeerResponse::decrypted_nonce);
    return codec;
  }
};
}
}