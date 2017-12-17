# Mediator
This document describes a specification for the _Mediator_, a server that is
trusted by Peers to mediate a handshake between them, in order to create a P2P
socket.

## Introduction
A Mediator server listens for [`Advertise`](protocol.md#a_advertise) messages
from Peers, and attempts to _register_ Peers by completing a
[Mediator handshake](protocol.md#mediator-handshake) with each Peer that sends
it an `Advertise`. It completes this handshake by verifying the identity of the
Peer, and registering its intent to connect with another specific Peer that may
or may not have already registered with the Mediator.

By completing the Mediator handshake with a Peer, the Mediator has verified the
Peers identity and registered the other Peer that it wishes to connect to. At
this point, there are two possibilities:
- *The Mediator has already registered the other Peer.* In this case, the
Mediator will provide the other Peer with the information it needs to connect to
this Peer, and instruct this Peer to close its connection to the Mediator and
wait for a connection from the other Peer.
- *The Mediator has not yet registered the other Peer.* When this happens, the
Mediator will keep its connection with this Peer open until the other Peer
registers, at which time it will provide this Peer with the information it needs
to connect to the other Peer.

By providing information to one Peer about the other Peer, the Mediator
facilitates the beginning of the
[Peer handshake](protocol.md#peer-handshake) between the two Peers, and is no
longer needed for the rest of the session.

## Coordinating a Peer handshake
When both Peers are online and wish to connect with each other, the Mediator 
must send a [`PeerDisconnect`](protocol.md#a_peer-disconnect) message to the 
Peer that registered with the Mediator last (the _Peer_). This message should 
include the IP and port of the Peer as observed by the Mediator. This will 
allow the Peer to set up a listening socket to listen for a P2P connection from
the Peer that registered with the Mediator first (the _Client_). 
 
The Mediator must also send a
[`PeerIdentification`](protocol.md#a_peer-identification) message to the Peer
that registered with the Mediator first (the _Client_). The Mediator includes
the other Peer's observed IP and port in this message. Any timeout or other 
communication error observed by the Mediator is considered an error, and the 
Mediator will close its connection to both Peers involved in the Peer
handshake in such an event.

The Mediator should guarantee that the `PeerDisconnect` message is received 
by the Peer before it sends the `PeerIdentification`, so that the Peer is 
already listening by the time the Client becomes aware of its IP and port.