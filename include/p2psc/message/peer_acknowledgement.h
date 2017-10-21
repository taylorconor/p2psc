#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct PeerAcknowledgement {
  static const MessageType type = kTypePeerAcknowledgement;
};
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::PeerAcknowledgement> {
  static codec::object_t<p2psc::message::PeerAcknowledgement> codec() {
    auto codec = codec::object<p2psc::message::PeerAcknowledgement>();
    return codec;
  }
};
}
}