#pragma once

#include <p2psc/message/message_format.h>
#include <p2psc/message/types.h>
#include <p2psc/version.h>
#include <spotify/json.hpp>

namespace p2psc {

template <class T> class Message {
public:
  Message(const T &payload) { _format.payload = payload; }

  const message::MessageFormat<T> format() const { return _format; }

private:
  message::MessageFormat<T> _format;
};
}