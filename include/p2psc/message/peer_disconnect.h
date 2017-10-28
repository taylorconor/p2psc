#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct PeerDisconnect {
  static const MessageType type = kTypePeerDisconnect;
  std::uint16_t port;
};

inline bool operator==(const PeerDisconnect &lhs, const PeerDisconnect &rhs) {
  return lhs.port == rhs.port;
}
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::PeerDisconnect> {
  static codec::object_t<p2psc::message::PeerDisconnect> codec() {
    auto codec = codec::object<p2psc::message::PeerDisconnect>();
    codec.required("port", &p2psc::message::PeerDisconnect::port);
    return codec;
  }
};
}
}