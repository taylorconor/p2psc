#include <include/util/id_to_address_store.h>

namespace p2psc {
namespace integration {

boost::optional<socket::SocketAddress>
IdToAddressStore::get(const std::string &id) {
  std::lock_guard<std::mutex> guard(_mutex);
  try {
    return _store.at(id);
  } catch (std::out_of_range e) {
    return boost::none;
  }
}

void IdToAddressStore::put(const std::string &id,
                           const socket::SocketAddress &socket_address) {
  std::lock_guard<std::mutex> guard(_mutex);
  _store.insert(
      std::pair<std::string, socket::SocketAddress>(id, socket_address));
}
}
}