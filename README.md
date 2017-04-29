# p2psc: peer-to-peer socket creation

p2psc is a small C++ library which enables the creation of network sockets between two peers anywhere on the internet, even behind NATs.

## Design overview
p2psc provides a simple interface to open a network socket between two peers, using a well-known technique called [hole punching](https://en.wikipedia.org/wiki/Hole_punching_(networking)).

Both peers advertise their public key to an intermediary server, known as a _mediator_. `Peer A` will also inform the mediator of `Peer B`'s public key, and vice-versa. When both peers are online, the mediator will negotiate a handshake between them, and each client will create a socket connection to the other peer. Each client will also verify the identity of the other peer.

## Interface
A socket can be created using a single static method call. All three parties involved in the handshake must be defined; the connecting client provides its own Keypair, a reference to the Peer it wishes to connect to, and a reference to the Mediator that will mediate the connection:

`void connectToPeer(const key::Keypair &, const Peer &, const Mediator &, const Callback &)`