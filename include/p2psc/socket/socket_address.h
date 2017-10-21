#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

namespace p2psc {
namespace socket {

const std::string local_ip = "127.0.0.1";

/**
 * This serves as an abstraction above sockaddr_in. <p2psc/socket.h> will
 * internally convert it to a sockaddr_in.
 */
class SocketAddress {
public:
  SocketAddress(const std::string &ip, std::uint16_t port)
      : _ip(ip), _port(port) {}

  std::string ip() const { return _ip; }
  std::uint16_t port() const { return _port; }

  bool operator==(const SocketAddress &other) {
    return this->port() == other.port() && this->ip() == other.ip();
  }

private:
  std::string _ip;
  std::uint16_t _port;

  friend std::ostream &operator<<(std::ostream &os,
                                  const SocketAddress &address) {
    os << address.ip() << ":" << address.port();
    return os;
  }

  friend bool operator==(const SocketAddress &address1,
                         const SocketAddress &address2) {
    return address1.port() == address2.port() && address1.ip() == address2.ip();
  }
};
}
}

namespace std {
template <> struct hash<p2psc::socket::SocketAddress> {
  size_t operator()(const p2psc::socket::SocketAddress &address) const {
    std::stringstream s;
    s << address;
    return hash<std::string>()(s.str());
  }
};
}