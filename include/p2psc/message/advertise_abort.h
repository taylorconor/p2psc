#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct AdvertiseAbort {
  static const MessageType type = kTypeAdvertiseAbort;
  std::string reason;
};
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::AdvertiseAbort> {
  static codec::object_t<p2psc::message::AdvertiseAbort> codec() {
    auto codec = codec::object<p2psc::message::AdvertiseAbort>();
    codec.required("reason", &p2psc::message::AdvertiseAbort::reason);
    return codec;
  }
};
}
}