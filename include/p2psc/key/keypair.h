#pragma once

#include <p2psc/crypto/pki.h>
#include <memory>

namespace p2psc {
namespace key {

class Keypair {
public:
  static Keypair generate();
  static Keypair from_pem(const std::string &path);

  std::string public_encrypt(const std::string &message) const;
  std::string private_decrypt(const std::string &message) const;
  std::string get_serialised_public_key() const;

private:
  Keypair(std::shared_ptr<crypto::PKI>);

  std::shared_ptr<crypto::PKI> _pki;
};
}
}