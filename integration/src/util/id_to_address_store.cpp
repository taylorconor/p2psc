#include <include/util/id_to_address_store.h>

namespace p2psc {
namespace integration {
namespace {
long long int current_time_ms() {
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch());
  return ms.count();
}
}

void IdToAddressStore::put(const std::string &id,
                           const socket::SocketAddress &socket_address) {
  {
    std::lock_guard<std::mutex> guard(_mutex);
    _store.insert(
        std::pair<std::string, socket::SocketAddress>(id, socket_address));
  }
  _cv.notify_all();
}

boost::optional<socket::SocketAddress>
IdToAddressStore::get(const std::string &id) {
  std::lock_guard<std::mutex> guard(_mutex);
  return _get(id);
}

boost::optional<socket::SocketAddress>
IdToAddressStore::await(const std::string &id, uint64_t ms) {
  std::unique_lock<std::mutex> lock(_mutex);

  const auto start_ms = current_time_ms();
  std::chrono::milliseconds wait_ms(ms);
  do {
    _cv.wait_for(lock, wait_ms);
    const auto address = _get(id);
    if (address) {
      return address;
    }
    wait_ms -= std::chrono::milliseconds(current_time_ms() - start_ms);
  } while (wait_ms.count() > 0);

  lock.unlock();
  return boost::none;
}

boost::optional<socket::SocketAddress>
IdToAddressStore::_get(const std::string &id) {
  try {
    return _store.at(id);
  } catch (std::out_of_range e) {
    return boost::none;
  }
}
}
}