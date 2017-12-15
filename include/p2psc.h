#pragma once

#include <p2psc/connection.h>

/**
 * This file defines the (very simple) API for p2psc. A single static method
 * is used to create sockets.
 */
namespace p2psc {

/**
 * Create a P2P socket with a known Peer
 *
 * @param keypair This Client's keypair (public and private key).
 * @param peer The Peer we wish to connect with (this includes their public
 * key).
 * @param mediator An identifier of a 3rd party Mediator service.
 * @param callback A function called when the socket has been successfully
 * created, or in the event of an error.
 */
static void connect(const key::Keypair &keypair, const Peer &peer,
                    const Mediator &mediator, const Callback &callback) {
  Connection::connect(keypair, peer, mediator, callback,
                      [](const SocketAddressOrFileDescriptor &param) {
                        if (param.has_socket_address()) {
                          return std::make_shared<Socket>(
                              param.socket_address());
                        } else {
                          return std::make_shared<Socket>(param.sock_fd());
                        }
                      });
}
}