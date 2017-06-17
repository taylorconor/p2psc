#pragma once

#include <p2psc/message/message_exception.h>
#include <p2psc/message/message_format.h>
#include <spotify/json/decode.hpp>
#include <spotify/json/decode_exception.hpp>
#include <string>

namespace p2psc {
namespace message {

template <class T>
static message::MessageFormat<T> decode(const std::string &message) {
  try {
    return spotify::json::decode<message::MessageFormat<T>>(message);
  } catch (const spotify::json::decode_exception &e) {
    throw message::MessageException(
        "Could not decode " + boost::typeindex::type_id<T>().pretty_name() +
        " message. Reason: " + std::string(e.what()));
  }
}

static MessageType decode_message_type(const std::string &message) {
  try {
    return spotify::json::decode<message::AnonymousMessageFormat>(message).type;
  } catch (const spotify::json::decode_exception &e) {
    throw message::MessageException(
        "Could not decode anonymous message. Reason: " + std::string(e.what()));
  }
}
}
}