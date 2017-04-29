#pragma once

#include <p2psc/key/public_key.h>

namespace p2psc {

/*
 * The very basic information needed to identify a Peer.
 */
struct Peer {
  const key::PublicKey public_key;

  Peer(const key::PublicKey &public_key) : public_key(public_key) {}
};
}