# Mediator
This document describes a specification for the _Mediator_, a server that is
trusted by Peers to mediate a handshake between them, in order to create a P2P
socket.

## Introduction
A Mediator server listens for [`Advertise`](protocol.md#a_advertise) messages
from Peers, and attempts to complete the
[Mediator handshake](protocol.md#mediator-handshake) with each Peer that sends
it an `Advertise`. For every new Client that registers, the Mediator should
check if the Peer that this Client wishes to connect with has already
registered. If so, the Mediator must check if that Peer also wishes to connect
with the Client. In this case, the Mediator will attempt to coordinate a
[Peer handshake](protocol.md#peer-handshake) between these two Peers.

## Coordinating a Peer handshake
When both Peers are online and wish to connect with each other, the Mediator
must send a [`PeerIdentification`](protocol.md#a_peer-identification) message to
the Peer that registered with the Mediator first (the _Client_). The Mediator
includes the other Peer's observed IP and port in this message. Any timeout or
other communication error observed by the Mediator is considered an error, and
the Mediator will close its connection to both Peers involved in the Peer
handshake in such an event.