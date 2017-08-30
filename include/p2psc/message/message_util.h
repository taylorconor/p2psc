#pragma once

#include <p2psc/log.h>
#include <p2psc/message.h>
#include <p2psc/socket.h>

namespace p2psc {
namespace message {
template <class T>
void send_and_log(std::shared_ptr<Socket> socket, const Message<T> &message) {
  const auto json = encode(message.format());
  socket->send(json);
  const auto message_type = message::message_type_string(message.format().type);
  LOG(level::Debug) << "Sending " << message_type << ": " << json;
}

template <class T> Message<T> receive_and_log(std::shared_ptr<Socket> socket) {
  const auto raw_message = socket->receive();
  auto message = message::decode<T>(raw_message);
  const auto message_type = message::message_type_string(message.type);
  LOG(level::Debug) << "Received " << message_type << ": " << raw_message;
  return message.payload;
}
}
}