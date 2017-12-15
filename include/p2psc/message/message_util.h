#pragma once

#include <p2psc/log.h>
#include <p2psc/message/message.h>
#include <p2psc/message/message_decoder.h>
#include <p2psc/socket/socket.h>

namespace p2psc {
namespace message {
template <class T>
inline void send_and_log(std::shared_ptr<Socket> socket,
                         const Message<T> &message) {
  const auto json = encode(message.format());
  socket->send(json);
  const auto message_type = message::message_type_string(message.format().type);
  LOG(level::Debug) << "Sending " << message_type << " to "
                    << socket->get_socket_address().ip() << ":"
                    << socket->get_socket_address().port() << ": " << json;
}

template <class T>
inline void log_message(const message::MessageFormat<T> &message,
                        const socket::SocketAddress &socket_address) {
  const auto message_type = message::message_type_string(message.type);
  LOG(level::Debug) << "Received " << message_type << " from "
                    << socket_address.ip() << ":" << socket_address.port()
                    << ": " << encode(message);
}

template <class T>
inline Message<T> receive_and_log(std::shared_ptr<Socket> socket) {
  const auto raw_message = socket->receive();
  auto message = message::decode<T>(raw_message);
  log_message(message, socket->get_socket_address());
  return message.payload;
}
}
}