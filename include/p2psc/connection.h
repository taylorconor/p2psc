#pragma once

#include <functional>

#include <p2psc/key/keypair.h>
#include <p2psc/mediator.h>
#include <p2psc/peer.h>
#include <p2psc/punched_peer.h>
#include <p2psc/socket.h>

namespace p2psc {

/**
 * The Connection class defines the (very simple) API for p2psc. A single static
 * method is used to create sockets.
 */
class Connection {
public:
  /*
   * Custom callback function to be called when the connection has been setup,
   * or when an error occurs.
   */
  using Callback = std::function<void(std::shared_ptr<Socket>)>;

  /*
   * Creates a socket with a known Peer.
   */
  static void connect(const key::Keypair &, const Peer &, const Mediator &,
                      const Callback &);

private:
  static void _execute_asynchronously(std::function<void()>);

  static void _handle_connection(const key::Keypair &, const Peer &,
                                 const Mediator &, const Callback &);
  static std::shared_ptr<Socket> _connect(const key::Keypair &, const Peer &,
                                          const Mediator &);
  static std::shared_ptr<Socket> _connect_as_client(const PunchedPeer &);
};
}