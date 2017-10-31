#include <src/util/key_to_identifier_store.h>

namespace p2psc {
namespace integration {
namespace {
long long int current_time_ms() {
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch());
  return ms.count();
}
}

void KeyToIdentifierStore::put(const std::string &id,
                               const PeerIdentifier &peer_identifier) {
  {
    std::lock_guard<std::mutex> guard(_mutex);
    _store.insert(std::pair<std::string, PeerIdentifier>(id, peer_identifier));
  }
  _cv.notify_all();
}

boost::optional<PeerIdentifier>
KeyToIdentifierStore::get(const std::string &id) {
  std::lock_guard<std::mutex> guard(_mutex);
  return _get(id);
}

boost::optional<PeerIdentifier>
KeyToIdentifierStore::await(const std::string &id, uint64_t ms) {
  std::unique_lock<std::mutex> lock(_mutex);

  const auto start_ms = current_time_ms();
  std::chrono::milliseconds wait_ms(ms);
  do {
    _cv.wait_for(lock, wait_ms);
    const auto identifier = _get(id);
    if (identifier) {
      return identifier;
    }
    wait_ms -= std::chrono::milliseconds(current_time_ms() - start_ms);
  } while (wait_ms.count() > 0);

  lock.unlock();
  return boost::none;
}

boost::optional<PeerIdentifier>
KeyToIdentifierStore::_get(const std::string &id) {
  try {
    return _store.at(id);
  } catch (std::out_of_range e) {
    return boost::none;
  }
}
}
}