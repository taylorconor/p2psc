#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct PeerSecret {
  static const MessageType type = kTypePeerSecret;
  std::string secret;
};
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::PeerSecret> {
  static codec::object_t<p2psc::message::PeerSecret> codec() {
    auto codec = codec::object<p2psc::message::PeerSecret>();
    codec.required("secret", &p2psc::message::PeerSecret::secret);
    return codec;
  }
};
}
}