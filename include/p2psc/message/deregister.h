#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct Deregister {
  static const MessageType type = kTypeDeregister;
  std::string peer_secret;
};
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::Deregister> {
  static codec::object_t<p2psc::message::Deregister> codec() {
    auto codec = codec::object<p2psc::message::Deregister>();
    codec.required("peer_secret", &p2psc::message::Deregister::peer_secret);
    return codec;
  }
};
}
}