#pragma once

#include <p2psc/message/types.h>
#include <spotify/json.hpp>

using namespace spotify::json;

namespace p2psc {
namespace message {

struct AdvertiseResponse {
  static const MessageType type = kTypeAdvertiseResponse;
  std::string nonce;
};
}
}

namespace spotify {
namespace json {
template <> struct default_codec_t<p2psc::message::AdvertiseResponse> {
  static codec::object_t<p2psc::message::AdvertiseResponse> codec() {
    auto codec = codec::object<p2psc::message::AdvertiseResponse>();
    codec.required("nonce", &p2psc::message::AdvertiseResponse::nonce);
    return codec;
  }
};
}
}