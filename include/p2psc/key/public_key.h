#pragma once

namespace p2psc {
namespace key {

class PublicKey {
public:
  static PublicKey from_string(const std::string &);
  static PublicKey generate();

  std::string encrypt(const std::string &) const;
  std::string serialise() const;

private:
  PublicKey(std::shared_ptr<crypto::PKI>);

  std::shared_ptr<crypto::PKI> _pki;
};
}
}