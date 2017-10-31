#include <crypto/rsa.h>
#include <p2psc/key/public_key.h>

namespace p2psc {
namespace key {

PublicKey PublicKey::from_string(const std::string &string) {
  return PublicKey(crypto::RSA::from_public_key(string));
}

PublicKey PublicKey::generate() { return PublicKey(crypto::RSA::generate()); }

PublicKey::PublicKey(std::shared_ptr<crypto::PKI> pki) : _pki(pki) {}

std::string PublicKey::encrypt(const std::string &message) const {
  return _pki->public_encrypt(message);
}

std::string PublicKey::serialise() const {
  return _pki->get_public_key_string();
}
}
}