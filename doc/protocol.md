# Protocol
This document describes the protocol used to create a socket between two Peers
using `p2psc`. All messages in the `p2psc` protocol are encoded in JSON format,
and consist of a message type, a protocol version, and a payload.

## Initialisation
Before attempting to create a socket between two Peers using `p2psc`, a
`Keypair` for both Peers must be either created or initialised. This is done
independently by both Peers. The protocol assumes that the connecting Peer, (the
_Client_), is aware of the other Peer's (the _Peer_) public key. The keypairs
are used so peers can prove their identities to each other, and can encrypt
their communication.

We also assume the presence of a Mediator, a server trusted by both Peers, to
mediate the handshake. A Mediator specification is outlined in
_**TODO: Mediator spec**_.

## Handshake
A socket creation handshake happens in two stages; first, the Client registers
with the Mediator (the _Mediator Handshake_). Then, the Mediator facilitates the
creation of a connection between the Client and its desired Peer (the _Peer 
Handshake_).

### Mediator handshake
A Peer advertises itself to the Mediator by sending an `Advertise` message:
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

The Mediator will then return one of three message types:
- If the Mediator considers the `Advertise` message valid, it will attempt to 
verify the identity of the Peer by sending an `AdvertiseChallenge` message:
```
{
    'type': kMessageTypeAdvertiseChallenge,
    'version': [p2psc version],
    'payload': {
        'encrypted_nonce': [Nonce encrypted with our_key]
    }
}
```

- If the `Advertise` message is errenous (for example if the Peer sends an 
`Advertise` with a `version` greater than that supported by the Mediator) an
`AdvertiseAbort` message will be sent to the Peer, along with a reason for 
the abort:
```
{
    'type': kMessageTypeAdvertiseAbort,
    'version': [p2psc version],
    'payload': {
        'reason': [String, reason for aborting the Mediator handshake]
    }
}
```

- If the Mediator wants the Peer to retry the `Advertise` message (for example 
if the `Advertise` came on a port that the Mediator determines the Peer will be
unable to bind to during the Peer Handshake) then an `AdvertiseRetry` message
is returned to the Peer, with a reason for the retry request:
```
{
    'type': kMessageTypeAdvertiseRetry,
    'version': [p2psc version],
    'payload': {
        'reason': [String, reason for requesting a retry]
    }
}
```

Finally, the Peer proves its identity to the Mediator by replying with an
`AdvertiseResponse` containing `nonce`, the `encrypted_nonce` decrypted using
the Peer's private key:
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

### Peer handshake
Once the Peer has proved its identity with the Mediator, the Mediator will
attempt to connect it to its specified Peer. This may not be immediate, as the
specified Peer may not yet have advertised to the Mediator.

The Peer who registers with the Mediator _first_ is considered to be the 
_Client_ for the Peer Handshake, while the other Peer is considered to be the
_Peer_.

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

Upon receipt of this message, the Peer verifies its own identity to the Client,
and attempts to verify the identity of the Client by replying with a 
`PeerChallengeResponse`:
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

Provided that the `decrypted_nonce` is correct, the Client will reply with a
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

If the Peer does not accept the Clients `PeerResponse`, it will close the
socket. Otherwise, it will reply with a `PeerAcknowledgement` message:
```
{
    'type': kMessageTypePeerAcknowledgement,
    'version': [p2psc version],
    'payload': {
    }
}
```

The Peer Handshake step is now complete, as both the Client and Peer are able to
communicate P2P.