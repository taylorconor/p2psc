# p2psc: peer-to-peer socket creation

p2psc is a small C++ library which enables the creation of network sockets
between two peers anywhere on the internet, even behind firewalls and NATs.

## Design overview
p2psc provides a simple interface to open a network socket between two peers,
using a well-known technique called
[hole punching](https://en.wikipedia.org/wiki/Hole_punching_(networking)).

Both peers advertise their public key to an intermediary server, known as a
_Mediator_. Each peer also informs the Mediator of the public key of the peer
they wish to create a socket with. When both peers have registered with the
Mediator and expressed interest to connect to each other, the Mediator will
negotiate a handshake between them, which results in a TCP socket being created
between the peers.

## Interface
A socket can be created using a single static method call. All three parties
involved in the handshake must be defined; the connecting peer provides its own
Keypair, a reference to the Peer it wishes to connect to, and a reference to the
Mediator that will mediate the connection:

`p2psc::connect(const key::Keypair &, const Peer &, const Mediator &, const
Callback &)`

A Callback is also provided, which will be called either with an error, or with
a socket that has been successfully connected to the specified Peer. It is
defined as:

`using Callback = std::function<void(p2psc::Error,
std::shared_ptr<p2psc::Socket>)>;`

## Example
Here's a basic example, which assumes we know the public key of the Peer we want
to connect to. In practice, sharing of the public key will likely happen in the
application layer above p2psc.
**TODO: the sad state of this example should motivate trimming the fat soon!**
```C++
#include <p2psc/connection.h>

int main() {
    auto my_keypair = p2psc::key::Keypair::from_pem("my_keypair.pem");
    auto their_key = p2psc::key::PublicKey::from_pem("their_public_key.pem");
    auto peer = p2psc::Peer(their_key);
    auto mediator = p2psc::Mediator("127.0.0.1", 1337);

    p2psc::Connection::connect(keypair, peer, mediator,
        [](p2psc::Error error, std::shared_ptr<p2psc::Socket> socket) {
          if (!error) {
            socket->send("Hallå!");
          }
        });
}
```

If everything goes well, we'll be able to send a message directly to the other
peer with the socket that has been created for us by p2psc!

## Mediator specification
p2psc provides a client-side library used to create a p2p socket. It does not
provide a Mediator server to mediate the socket creation. The [Mediator
specification](doc/mediator.md) can be used to create a Mediator server.

## Protocol specification
Detailed specification of the protocol can be found [here](doc/protocol.md).