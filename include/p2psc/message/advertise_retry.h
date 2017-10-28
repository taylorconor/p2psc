#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct AdvertiseRetry {
  static const MessageType type = kTypeAdvertiseRetry;
  std::string reason;
};

inline bool operator==(const AdvertiseRetry &lhs, const AdvertiseRetry &rhs) {
  return lhs.reason == rhs.reason;
}
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::AdvertiseRetry> {
  static codec::object_t<p2psc::message::AdvertiseRetry> codec() {
    auto codec = codec::object<p2psc::message::AdvertiseRetry>();
    codec.required("reason", &p2psc::message::AdvertiseRetry::reason);
    return codec;
  }
};
}
}