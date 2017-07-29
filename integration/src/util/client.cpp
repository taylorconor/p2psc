#include <include/util/client.h>
#include <p2psc/connection.h>

namespace p2psc {
namespace integration {
namespace util {
std::shared_ptr<Socket> Client::connect() {
  std::shared_ptr<Socket> socket;
  const p2psc::Connection::Callback callback = [&socket](
      std::shared_ptr<Socket> created_socket) { socket = created_socket; };
  // TODO: explicitly specify that this call be blocking
  Connection::connectToPeer(_keypair, _peer, _mediator, callback);
  return socket;
}
}
}
}