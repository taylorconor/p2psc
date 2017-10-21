#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct AdvertiseChallenge {
  static const MessageType type = kTypeAdvertiseChallenge;
  std::string encrypted_nonce;
};
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::AdvertiseChallenge> {
  static codec::object_t<p2psc::message::AdvertiseChallenge> codec() {
    auto codec = codec::object<p2psc::message::AdvertiseChallenge>();
    codec.required("encrypted_nonce",
                   &p2psc::message::AdvertiseChallenge::encrypted_nonce);
    return codec;
  }
};
}
}