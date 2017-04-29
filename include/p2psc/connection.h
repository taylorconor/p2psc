#pragma once

#include <functional>

#include <p2psc/key/keypair.h>
#include <p2psc/mediator.h>
#include <p2psc/peer.h>

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
  using Callback = std::function<void()>;

  /*
   * Creates a socket with a known Peer.
   */
  static void connectToPeer(const key::Keypair &, const Peer &,
                            const Mediator &, const Callback &);

private:
  static void _executeAsynchronously(std::function<void()>);

  static void _connectToPeer(const key::Keypair &, const Peer &,
                             const Mediator &, const Callback &);
};
}