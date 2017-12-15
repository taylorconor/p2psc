#include <src/util/client.h>

namespace p2psc {
namespace integration {
namespace util {
std::shared_ptr<Socket> Client::connect_async() {
  std::shared_ptr<Socket> socket;
  const p2psc::Callback callback =
      [&socket](Error error, std::shared_ptr<Socket> created_socket) {
        socket = created_socket;
      };
  p2psc::connect(_keypair, _peer, _mediator, callback);
  return socket;
}

std::shared_ptr<Socket> Client::connect_sync(uint64_t timeout_ms) {
  std::shared_ptr<Socket> socket;
  std::mutex mutex;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(mutex);
  const p2psc::Callback callback =
      [&socket, &cv](Error error, std::shared_ptr<Socket> created_socket) {
        socket = created_socket;
        cv.notify_all();
      };
  p2psc::connect(_keypair, _peer, _mediator, callback);
  std::chrono::milliseconds wait_ms(timeout_ms);
  cv.wait_for(lock, wait_ms);
  return socket;
}
}
}
}