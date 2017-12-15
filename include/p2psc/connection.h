#pragma once

#include <functional>

#include <p2psc/error.h>
#include <p2psc/key/keypair.h>
#include <p2psc/mediator.h>
#include <p2psc/peer.h>
#include <p2psc/socket/socket.h>
#include <p2psc/socket_creator.h>

namespace p2psc {

/*
 * Custom callback function to be called when the connection has been setup,
 * or when an error occurs.
 */
// TODO(taylorconor): Socket should be the interface here.
using Callback = std::function<void(Error, std::shared_ptr<Socket>)>;

class Connection {
public:
  static void connect(const key::Keypair &, const Peer &, const Mediator &,
                      const Callback &, const SocketCreator &);

private:
  static void _execute_asynchronously(std::function<void()>);

  static void _handle_connection(const key::Keypair &, const Peer &,
                                 const Mediator &, const Callback &,
                                 const SocketCreator &);
  static std::shared_ptr<Socket> _connect(const key::Keypair &, const Peer &,
                                          const Mediator &,
                                          const SocketCreator &);
};
}