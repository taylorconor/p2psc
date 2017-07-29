#pragma once

#include <functional>

#include <p2psc/key/keypair.h>
#include <p2psc/mediator.h>
#include <p2psc/peer.h>
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
  static void connectToPeer(const key::Keypair &, const Peer &,
                            const Mediator &, const Callback &);

private:
  // TODO: it could be a good idea to have a connectToPeer that accepts a custom
  // TODO: executor so we can run tests synchronously and guarantee determinism.
  static void _executeAsynchronously(std::function<void()>);
  static void _executeSynchronously(std::function<void()>);

  static void _handleConnection(const key::Keypair &, const Peer &,
                                const Mediator &, const Callback &);
  static std::shared_ptr<Socket> _connectToPeer(const key::Keypair &,
                                                const Peer &, const Mediator &);
};
}