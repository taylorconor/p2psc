#include "local_listening_socket.h"

#include <arpa/inet.h>
#include <p2psc/log.h>

namespace p2psc {
namespace socket {
namespace {

int create_socket_fd(u_int32_t port) {
  int sockfd;
  struct sockaddr_in sock_addr;

  sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    throw std::runtime_error("Failed to open socket");
  }

  const int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    throw new std::runtime_error("Failed to set SO_REUSEADDR");
  }

  bzero((char *)&sock_addr, sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_addr.s_addr = inet_addr(local_ip.c_str());
  sock_addr.sin_port = htons(port);
  if (bind(sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
    throw std::runtime_error("Failed to bind to port " + std::to_string(port) +
                             ". Reason: " + strerror(errno));
  }
  return sockfd;
}

uint16_t port_from_socket(int sockfd) {
  struct sockaddr_in sock_addr;

  bzero(&sock_addr, sizeof(sock_addr));
  socklen_t len = sizeof(sock_addr);
  getsockname(sockfd, (struct sockaddr *)&sock_addr, &len);
  return ntohs(sock_addr.sin_port);
}
}

LocalListeningSocket::LocalListeningSocket()
    : _sockfd(create_socket_fd(INADDR_ANY)), _port(port_from_socket(_sockfd)) {
  listen(_sockfd, 5);
  _is_open = true;
}

LocalListeningSocket::LocalListeningSocket(uint16_t port)
    : _sockfd(create_socket_fd(port)), _port(port) {
  listen(_sockfd, 5);
  _is_open = true;
}

LocalListeningSocket::~LocalListeningSocket() { close(); }

std::shared_ptr<Socket> LocalListeningSocket::accept() const {
  BOOST_ASSERT(_is_open);
  int session_fd = ::accept(_sockfd, NULL, NULL);
  if (session_fd < 0) {
    return nullptr;
  }
  return std::make_shared<Socket>(session_fd);
}

void LocalListeningSocket::close() {
  if (_is_open) {
    ::close(_sockfd);
    _is_open = false;
  }
}

socket::SocketAddress LocalListeningSocket::get_socket_address() const {
  return socket::SocketAddress(local_ip, _port);
}
}
}