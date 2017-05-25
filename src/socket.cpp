#include <boost/assert.hpp>
#include <p2psc/socket.h>

namespace p2psc {
namespace {
const int RECV_BUF_SIZE = 1024;
}

Socket::Socket(const socket::SocketAddress &socket_address) : _is_open(false) {
  memset(&_address, 0, sizeof(_address));
  _address.sin_family = AF_INET;
  _address.sin_port = htons(socket_address.port);
  inet_pton(AF_INET, socket_address.ip.c_str(), &(_address.sin_addr));
  _sock_fd = ::socket(PF_INET, SOCK_STREAM, 0);
  _connect();
}

Socket::~Socket() {
  if (_is_open) {
    ::close(_sock_fd);
    _is_open = false;
  }
}

void Socket::send(const std::string &message) {
  _check_is_open();
  const auto size = ::send(_sock_fd, &message[0], message.size(), 0);
  if (size != message.length()) {
    std::stringstream fmt;
    fmt << "Unexpected data send length. Expected: " << message.length()
        << ", actual: " << size;
    if (errno) {
      fmt << ". Reason: " << strerror(errno);
    }
    throw socket::SocketException(fmt.str());
  }
}

std::string Socket::receive() {
  _check_is_open();
  std::string received_data;
  char receive_buffer[RECV_BUF_SIZE];
  ssize_t received_bytes;
  do {
    received_bytes = recv(_sock_fd, receive_buffer, RECV_BUF_SIZE, 0);
    received_data +=
        std::string(receive_buffer, receive_buffer + received_bytes);
  } while (received_bytes == RECV_BUF_SIZE);

  if (received_bytes == 0) {
    throw socket::SocketException("receive failed: Peer closed connection");
  }
  return received_data;
}

void Socket::_connect() {
  BOOST_ASSERT(!_is_open);
  if (::connect(_sock_fd, (struct sockaddr *)&_address, sizeof(_address)) !=
      0) {
    throw socket::SocketException(
        "Failed to connect to " + std::string(inet_ntoa(_address.sin_addr)) +
        ":" + std::to_string(ntohs(_address.sin_port)));
  }
  _is_open = true;
}

void Socket::_check_is_open() {
  if (!_is_open) {
    throw socket::SocketException("Socket is closed");
  }
}
}