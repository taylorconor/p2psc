#pragma once

#include <cassert>
#include <iostream>
#include <memory>

#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <p2psc/version.h>

namespace p2psc {
namespace crypto {

class RSA {
public:
  static constexpr int max_data_size(std::uint16_t key_size) {
    return (key_size / 8) - 11;
  };

  static std::shared_ptr<RSA> from_public_key(const std::string &);
  static std::shared_ptr<RSA> generate();

  std::string public_encrypt(const std::string &) const;
  std::string public_decrypt(const std::string &) const;
  std::string private_encrypt(const std::string &) const;
  std::string private_decrypt(const std::string &) const;

  std::string get_public_key_string() const;

  ~RSA();

private:
  RSA(::RSA *);
  ::RSA *_key;
};
}
}