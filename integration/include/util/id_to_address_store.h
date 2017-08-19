#pragma once

#include <boost/optional.hpp>
#include <map>
#include <mutex>
#include <p2psc/socket/socket_address.h>
#include <string>

namespace p2psc {
namespace integration {

class IdToAddressStore {
public:
  void put(const std::string &id, const socket::SocketAddress &socket_address);
  boost::optional<socket::SocketAddress> get(const std::string &id);
  boost::optional<socket::SocketAddress> await(const std::string &id,
                                               uint64_t ms);

private:
  boost::optional<socket::SocketAddress> _get(const std::string &id);

  std::map<std::string, socket::SocketAddress> _store;
  std::mutex _mutex;
  std::condition_variable _cv;
};
}
}