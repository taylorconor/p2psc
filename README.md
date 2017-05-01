# p2psc: peer-to-peer socket creation

p2psc is a small C++ library which enables the creation of network sockets
between two peers anywhere on the internet, even behind NATs.

## Design overview
p2psc provides a simple interface to open a network socket between two peers,
using a well-known technique called
[hole punching](https://en.wikipedia.org/wiki/Hole_punching_(networking)).

Both peers advertise their public key to an intermediary server, known as a
_Mediator_. Each peer also informs the Mediator of the public key of the peer
they wish to create a socket with. When both peers have registered with the
Mediator and expressed interest to connect to each other, the Mediator will
negotiate a handshake between them, which results in a TCP socket being created
between both peers.

## Interface
A socket can be created using a single static method call. All three parties
involved in the handshake must be defined; the connecting peer provides its own
Keypair, a reference to the Peer it wishes to connect to, and a reference to the
Mediator that will mediate the connection:

`void connectToPeer(const key::Keypair &, const Peer &, const Mediator &, const
Callback &)`