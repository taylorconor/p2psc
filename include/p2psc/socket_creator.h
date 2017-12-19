#pragma once

#include <functional>
#include <p2psc/socket/socket.h>
#include <p2psc/socket/socket_address.h>
#include <memory>

namespace p2psc {
class SocketAddressOrFileDescriptor {
public:
  SocketAddressOrFileDescriptor(const socket::SocketAddress &socket_address)
      : _socket_address(&socket_address) {
    _has_socket_address = true;
  }

  SocketAddressOrFileDescriptor(int sock_fd)
      : _socket_address(nullptr), _sock_fd(sock_fd) {
    _has_socket_address = false;
  }

  bool has_socket_address() const { return _has_socket_address; }

  const socket::SocketAddress &socket_address() const {
    return *_socket_address;
  }

  int sock_fd() const { return _sock_fd; }

private:
  bool _has_socket_address;
  const socket::SocketAddress *_socket_address;
  int _sock_fd;
};

/*
 * A function that can create sockets. This allows us to make p2psc create
 * mocked sockets for testing.
 */
using SocketCreator = std::function<std::shared_ptr<Socket>(
    const SocketAddressOrFileDescriptor &)>;
}