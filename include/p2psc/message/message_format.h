#pragma once

#include <p2psc/message/anonymous_message_format.h>
#include <p2psc/message/types.h>
#include <p2psc/version.h>
#include <spotify/json.hpp>

namespace p2psc {
namespace message {

/**
 * MessageFormat defines the format of all messages sent and received by p2psc.
 */
template <class T> struct MessageFormat : AnonymousMessageFormat {
  message::MessageType type = T::type;
  T payload;
};
}
}

namespace spotify {
namespace json {
template <class T> struct default_codec_t<p2psc::message::MessageFormat<T>> {
  static codec::object_t<p2psc::message::MessageFormat<T>> codec() {
    auto codec = codec::object<p2psc::message::MessageFormat<T>>();
    codec.required("type", &p2psc::message::MessageFormat<T>::type);
    codec.required("payload", &p2psc::message::MessageFormat<T>::payload);
    return codec;
  }
};
}
}