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
A socket creation handshake happens in three stages; first, the Client registers
with the Mediator. Then, the Mediator connects the Client to its desired Peer.
Finally, the Client deregisters both itself and the Peer from the Mediator upon
successful handshake.

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
`AdvertiseChallenge` message. This message contains a `secret`, which will be
used later during [Mediator deregistration](#mediator-deregistration):
```
{
    'type': kMessageTypeAdvertiseChallenge,
    'version': [p2psc version],
    'payload': {
        'encrypted_nonce': [Nonce encrypted with our_key],
        'secret': [Nonce]
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
Peer may not yet have advertised to the Mediator. The Client will listen
indefinitely (or within some reasonably long timeout) for messages on the socket
it created with the Mediator.

<a id="a_peer-identification"></a>
When both the Client and Peer are registered with the Mediator, the Mediator
sends a `PeerIdentification` message to the Client which registered with the
Mediator first:
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
Mediator. The Peer should still be listening on that port for a
`PeerIdentification` message from the Mediator; however it will not receive one.
Instead, the Client will use the Peer's IP and port to send a `PeerChallenge`
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

If this is accepted by the Peer, the Peer will send the Client the `secret` it
obtained from the Mediator during the [Mediator handshake](#mediator-handshake).
This allows the Client to prove to the Mediator that the Peer trusts the Client.
It is sent in a `PeerSecret` message:
```
{
    'type': kMessageTypePeerSecret,
    'version': [p2psc version],
    'payload': {
        'secret': [Peer secret]
    }
}
```

the Peer handshake step is now complete, as both the Client and Peer are now
able to communicate P2P.

### Mediator deregistration
<a id="a_deregister"></a>
Once both the Peer and Client have accepted each others identity, the Client
will send a `Deregister` message to the Mediator. This acknowledges the success
of the Peer handshake, and prompts the Mediator to deregister both the Peer and
Client:
```
{
    'type': kMessageTypeDeregister,
    'version': [p2psc version],
    'payload': {
        'secret': [Peer secret]
    }
}
```

This prompts the Mediator to remove all temporarily stored port/IP references to
both the Client and the Peer. If either wish to create a new socket with another
Peer, they will need to create a brand new handshake with the Mediator.