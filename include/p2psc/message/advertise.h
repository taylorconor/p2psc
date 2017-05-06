#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct Advertise {
  static const MessageType type = kTypeAdvertise;
  std::string our_key;
  std::string their_key;
};
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::Advertise> {
  static codec::object_t<p2psc::message::Advertise> codec() {
    auto codec = codec::object<p2psc::message::Advertise>();
    codec.required("our_key", &p2psc::message::Advertise::our_key);
    codec.required("their_key", &p2psc::message::Advertise::their_key);
    return codec;
  }
};
}
}