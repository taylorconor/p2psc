#include <arpa/inet.h>
#include <include/util/local_listening_socket.h>

namespace p2psc {
namespace integration {
namespace util {
namespace {
const char *LOCAL_IP = "127.0.0.1";

int random_socket() {
  int sockfd;
  struct sockaddr_in sock_addr;

  sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    throw std::runtime_error("Failed to open socket");
  }

  bzero((char *)&sock_addr, sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_addr.s_addr = inet_addr(LOCAL_IP);
  // randomly selected port
  sock_addr.sin_port = htons(INADDR_ANY);
  if (bind(sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
    throw std::runtime_error("ERROR on binding");
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
    : _sockfd(random_socket()), _port(port_from_socket(_sockfd)) {
  listen(_sockfd, 5);
}

LocalListeningSocket::~LocalListeningSocket() { close(); }

std::shared_ptr<Socket> LocalListeningSocket::accept() {
  int session_fd = ::accept(_sockfd, NULL, NULL);
  if (session_fd < 0) {
    return nullptr;
  }
  return std::make_shared<Socket>(session_fd);
}

void LocalListeningSocket::close() { ::close(_sockfd); }

socket::SocketAddress LocalListeningSocket::get_socket_address() {
  return socket::SocketAddress(LOCAL_IP, _port);
}
}
}
}