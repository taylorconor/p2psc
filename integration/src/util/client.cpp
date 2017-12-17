#include <src/util/client.h>

namespace p2psc {
namespace integration {
namespace util {
std::shared_ptr<StatefulSocket> Client::connect_async() {
  std::shared_ptr<StatefulSocket> socket;
  const p2psc::Callback callback =
      [&socket](Error error, std::shared_ptr<Socket> created_socket) {
        socket = std::static_pointer_cast<StatefulSocket>(created_socket);
      };
  p2psc::Connection::connect(
      _keypair, _peer, _mediator, callback,
      [](const SocketAddressOrFileDescriptor &param) {
        if (param.has_socket_address()) {
          return std::make_shared<StatefulSocket>(param.socket_address());
        } else {
          return std::make_shared<StatefulSocket>(param.sock_fd());
        }
      });
  return socket;
}

std::shared_ptr<StatefulSocket> Client::connect_sync(uint64_t timeout_ms) {
  std::shared_ptr<StatefulSocket> socket;
  std::mutex mutex;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mutex);
  const p2psc::Callback callback =
      [&socket, &cv](Error error, std::shared_ptr<Socket> created_socket) {
        socket = std::static_pointer_cast<StatefulSocket>(created_socket);
        cv.notify_all();
      };
  p2psc::Connection::connect(
      _keypair, _peer, _mediator, callback,
      [](const SocketAddressOrFileDescriptor &param) {
        if (param.has_socket_address()) {
          return std::make_shared<StatefulSocket>(param.socket_address());
        } else {
          return std::make_shared<StatefulSocket>(param.sock_fd());
        }
      });
  std::chrono::milliseconds wait_ms(timeout_ms);
  cv.wait_for(lock, wait_ms);
  return socket;
}
}
}
}