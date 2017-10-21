# Protocol
This document describes the protocol used to create a socket between two peers
using `p2psc`. All messages in the `p2psc` protocol are encoded in JSON format,
and consist of a message type, a protocol version, and a payload.

## Initialisation
Before attempting to create a socket between two peers using `p2psc`, a
`Keypair` for both peers must be either created or initialised. This is done
independently by both peers. The protocol assumes that the connecting peer, (the
_Client_), is aware of the other peer's (the _Peer_) public key. The keypairs
are used so peers can prove their identities to each other, and can encrypt
their communication.

We also assume the presence of a Mediator, a server trusted by both peers, to
mediate the handshake. A Mediator specification is outlined in
_**TODO: Mediator spec**_.

## Handshake
A socket creation handshake happens in two stages; first, the Client registers
with the Mediator. Then, the Mediator facilitates the creation of a 
connection between the Client and its desired Peer.

### Mediator handshake
A Client advertises itself to the Mediator by sending an `Advertise` message:
```
{
    'type': kMessageTypeAdvertise,
    'version': [p2psc version],
    'payload': {
        'our_key': [Public Key],
        'their_key': [Public Key]
     }
}
```

The Mediator then attempts to verify the identity of the Client by sending an
`AdvertiseChallenge` message:
```
{
    'type': kMessageTypeAdvertiseChallenge,
    'version': [p2psc version],
    'payload': {
        'encrypted_nonce': [Nonce encrypted with our_key]
    }
}
```

Finally, the Client proves its identity to the Mediator by replying with an
`AdvertiseResponse` containing `nonce`, the `encrypted_nonce` decrypted using
the Client's private key:
```
{
    'type': kMessageTypeAdvertiseResponse,
    'version': [p2psc version],
    'payload': {
        'nonce': [Decrypted encrypted_nonce from AdvertiseChallenge]
    }
}
```

The Mediator handles an invalid `AdvertiseResponse` by closing the socket.

Note that the words Peer and Client depend on perspective. Both the Peer and
Client will at some point in time independently complete a Mediator handshake
as Clients. During Peer handshake, the Mediator considers the Client to be the
peer which registered with the Mediator _first_.

### Peer handshake
Once the Client has proved its identity with the Mediator, the Mediator will
attempt to connect it to it's desired Peer. This may not be immediate, as the
Peer may not yet have advertised to the Mediator.

<a id="a_peer-disconnect"></a>
When both the Client and Peer are registered with the Mediator, the Mediator
sends a `PeerDisconnect` message to the Peer:
```
{
    'type': kMessageTypePeerDisconnect,
    'version': [p2psc version],
    'payload': {
        'port': [uint16_t port number]
    }
}
```

This message lets the Peer know that it should close its connection with the
Mediator and create a new socket bound to `port` to listen for a connection
from the Client. `port` will in this case be the same port that the Peer used
to communicate with the Mediator. Since the Peer was the active closer of its
socket with the Mediator, the socket's state will be in `TIME_WAIT`, and so any
firewall or NAT gateway should keep a port mapping to the Peers host machine
active.

<a id="a_peer-identification"></a>
At the same time, the Mediator will send a `PeerIdentification` message to 
the Client:
```
{
    'type': kMessageTypePeerIdentification,
    'version': [p2psc version],
    'payload': {
        'ip': [String IP address],
        'port': [uint16_t port number]
    }
}
```

In this case, `ip` and `port` are the IP and port of the Peer as observed by the
Mediator. The Peer will have received a `PeerDisconnect` message from the 
Mediator and should now be listening on `port`. The Client will use the Peer's 
IP and port from the `PeerIdentification` message to send a `PeerChallenge` 
message to the Peer:
```
{
    'type': kMessageTypePeerChallenge,
    'version': [p2psc version],
    'payload': {
        'encrypted_nonce': [Nonce encrypted with Client's public key]
    }
}
```

Upon receipt of this message, the Peer verifies it's own identity and attempts
to verify the identity of the Client by replying with a `PeerChallengeResponse`:
```
{
    'type': kMessageTypePeerChallengeResponse,
    'version': [p2psc version],
    'payload': {
        'encrypted_nonce': [Nonce encrypted with Peer's public key],
        'decrypted_nonce': [Decrypted encrypted_nonce from PeerChallenge]
    }
}
```

Providing the `decrypted_nonce` is correct, the Client will reply with a
`PeerResponse` message:
```
{
    'type': kMessageTypePeerResponse,
    'version': [p2psc version],
    'payload': {
        'decrypted_nonce': [Decrypted encrypted_nonce from PeerChallengeResponse]
    }
}
```

the Peer handshake step is now complete, as both the Client and Peer are now
able to communicate P2P.