#pragma once

#include <boost/optional.hpp>
#include <include/util/peer_identifier.h>
#include <map>
#include <mutex>
#include <p2psc/socket/socket_address.h>
#include <string>

namespace p2psc {
namespace integration {

class KeyToIdentifierStore {
public:
  void put(const std::string &key, const PeerIdentifier &peer_identifier);
  boost::optional<PeerIdentifier> get(const std::string &key);
  boost::optional<PeerIdentifier> await(const std::string &key, uint64_t ms);

private:
  boost::optional<PeerIdentifier> _get(const std::string &id);

  std::map<std::string, PeerIdentifier> _store;
  std::mutex _mutex;
  std::condition_variable _cv;
};
}
}