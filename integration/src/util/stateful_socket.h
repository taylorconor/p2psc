#pragma once

#include <p2psc/socket/socket.h>
#include <vector>

namespace p2psc {
namespace integration {
namespace util {
class StatefulSocket : public Socket {
public:
  StatefulSocket(const socket::SocketAddress &socket_address)
      : Socket(socket_address) {}
  StatefulSocket(int sock_fd) : Socket(sock_fd) {}

  void send(const std::string &) override;
  std::string receive() override;

  const std::vector<std::string> &get_received_messages() const {
    return received_messages;
  };

  const std::vector<std::string> &get_sent_messages() const {
    return sent_messages;
  };

private:
  std::vector<std::string> received_messages;
  std::vector<std::string> sent_messages;
};
}
}
}
