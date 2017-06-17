#include <p2psc/crypto/rsa.h>
#include <p2psc/key/keypair.h>

namespace p2psc {
namespace key {

Keypair Keypair::generate() { return Keypair(crypto::RSA::generate()); }

Keypair Keypair::from_pem(const std::string &path) {
  return Keypair(crypto::RSA::from_pem(path));
}

Keypair::Keypair(std::shared_ptr<crypto::PKI> pki) : _pki(pki) {}

std::string Keypair::public_encrypt(const std::string &message) const {
  return _pki->public_encrypt(message);
}

std::string Keypair::private_decrypt(const std::string &encrypted) const {
  return _pki->private_decrypt(encrypted);
}

std::string Keypair::get_serialised_public_key() const {
  return _pki->get_public_key_string();
}
}
}