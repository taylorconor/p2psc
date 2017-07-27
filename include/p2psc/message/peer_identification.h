#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct PeerIdentification {
  static const MessageType type = kTypePeerIdentification;
  std::string ip;
  std::uint16_t port;
};
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::PeerIdentification> {
  static codec::object_t<p2psc::message::PeerIdentification> codec() {
    auto codec = codec::object<p2psc::message::PeerIdentification>();
    codec.required("ip", &p2psc::message::PeerIdentification::ip);
    codec.required("port", &p2psc::message::PeerIdentification::port);
    return codec;
  }
};
}
}