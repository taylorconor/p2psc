#include "stateful_socket.h"

namespace p2psc {
namespace integration {
namespace util {
void StatefulSocket::send(const std::string &message) {
  Socket::send(message);
  sent_messages.push_back(message);
}

std::string StatefulSocket::receive() {
  const auto message = Socket::receive();
  received_messages.push_back(message);
  return message;
}
}
}
}